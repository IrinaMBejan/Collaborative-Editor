#include <regex>
#include <fstream>
#include <algorithm>
#include <iostream>

#include "handlerthread.h"
#include "utils.h"
#include "csv.h"


#define MAX_EMPTY 10


using namespace std; //// remove me


static constexpr const char* retrieve_list = "retrieve files";
static constexpr const char* operations_init = "operations start";
static constexpr const char* operations_close = "operations close";
static constexpr const char* operation_insert = "insert ([0-9]+) (.*?)";
static constexpr const char* operation_delete = "delete ([0-9]+) ([0-9]+)";
static constexpr const char* create_file = 
  "create file ([a-zA-Z0-9]+.[a-zA-Z0-9]+)";
static constexpr const char* download_file = 
  "download file ([a-zA-Z0-9]+.[a-zA-Z0-9]+)";
static constexpr const char* edit_file = 
  "edit file ([a-zA-Z0-9]+.[a-zA-Z0-9]+)";
static constexpr const char* folder = 
  "/home/ina/Repos/Irina/Collaborative-Editor/Server/files/";

void HandlerThread::Start()
{
  std::string buffer;
  buffer.clear();
  
  int tries = 0;
  while(1)
  {
    if (Read(client, buffer))
    {
      HandleMessage(buffer);
      buffer.clear();
    } else {
        tries += 1;
        if (tries >= MAX_EMPTY)
            break;
    }
  }
}

void HandlerThread::HandleMessage(const std::string& buffer)
{
  cout << "in dispatch: " << buffer << endl;
  if (!buffer.size())
    return;

  std::smatch matches;

  std::regex retrieve_list_reg(retrieve_list);
  std::regex download_file_reg(download_file);
  std::regex create_file_reg(create_file);
  std::regex edit_file_reg(edit_file);
  std::regex operations_init_reg(operations_init);
  std::regex operations_close_reg(operations_close);
  std::regex operations_insert_reg(operation_insert);
  std::regex operations_delete_reg(operation_delete);

  if (std::regex_match(buffer, retrieve_list_reg))
  {
    HandleRetrieveRequest();
  }
  else if (std::regex_match(buffer, matches, create_file_reg))
  {
    if (matches.size() == 2)
    {
      std::string filename = matches[1].str();
      HandleCreateFileRequest(filename);
    }
  }
  else if (std::regex_match(buffer, matches, download_file_reg))
  {
    if (matches.size() == 2)
    {
      std::string filename = matches[1].str();
      HandleDownloadRequest(filename);
    }
  }
  else if (std::regex_match(buffer, matches, edit_file_reg))
  {
    if (matches.size() == 2)
    {
      std::string filename = matches[1].str();
      HandleEditRequest(filename);
    }
  }
  else if (std::regex_match(buffer, operations_init_reg))
  {
    HandleOperationStart();
  }
  else if (std::regex_match(buffer, operations_close_reg))
  {
    HandleOperationClose();
  }
  else if (std::regex_match(buffer, matches, operations_insert_reg))
  {
    if (matches.size() == 3)
    {
      int position = std::stoi(matches[1].str());
      std::string text = matches[2].str();
      HandleInsertOperation(position, text);
    }
  }
  else if (std::regex_match(buffer, matches, operations_delete_reg))
  {
    if (matches.size() == 3)
    {
      int position = std::stoi(matches[1].str());
      int size = std::stoi(matches[2].str());
      HandleDeleteOperation(position, size);
    }
  }
  else 
  {
    printf("Don't know how to treat this: %s\n", buffer.c_str());
    Write(client, "Rejected");
  }
}

void HandlerThread::HandleOperationStart()
{
  (sessionIdx != -1) ?
    Write(client, "Succes") :
    Write(client, "Rejected");
}

void HandlerThread::HandleOperationClose()
{
  if (ExitSession(sessionIdx))
  {
    sessionIdx = -1;
    Write(client, "Succes");
  }
}

void HandlerThread::HandleInsertOperation(int position, std::string text)
{
  std::lock_guard<std::mutex> guard((*sessions)[sessionIdx]->content_mutex);
  
  Session* session = (*sessions)[sessionIdx];
  session->content.insert(position,text);

  Write(client,"Succes");
}

void HandlerThread::HandleDeleteOperation(int position, int length)
{
  std::lock_guard<std::mutex> guard((*sessions)[sessionIdx]->content_mutex);
  
  Session* session = (*sessions)[sessionIdx];
  session->content.erase(position,length);

  Write(client, "Succes");
}

void HandlerThread::HandleRetrieveRequest()
{
  std::string filepath = folder;
  filepath += FILE_LIST;
  
  CSVReader reader(filepath);
  const std::vector<CSVRow> files = reader.GetData();

  std::string res = "";
  for (int idx = 0; idx < (int)files.size(); ++idx)
  {
    res += files[idx].GetAt(0);
    res += ",";
    res += files[idx].GetAt(1);

    if (idx +1 < (int)files.size())
      res += ",";
  }

  printf("List of files sent is %s\n", res.c_str());

  Write(client, "Succes");
  Write(client, res);
}

void HandlerThread::HandleCreateFileRequest(const std::string& filename)
{
  std::string filepath = folder + filename;
  std::fstream file;                                                                                  
  file.open(filepath, std::ios::out | std::ios::trunc); 
  file.close();

  CSVRow listRow;
  listRow.Add(filename);
  listRow.Add("0");

  std::string fileListPath = folder;
  fileListPath += FILE_LIST;
  CSVWriter listUpdate(fileListPath);
  listUpdate.AddRow(listRow);

  InitSession();

  Write(client, "Succes");
}

void HandlerThread::HandleDownloadRequest(const std::string& filename)
{
  std::string filepath = folder+filename;
  std::string tmp;  
  std::ifstream t;
  std::stringstream buffer;
  
  t.open(filepath);
  while (!t.eof())
  {
    getline(t, tmp);
    buffer << tmp;
  }
  t.close();
  
  Write(client, "Succes");
  Write(client, buffer.str());
}

void HandlerThread::HandleEditRequest(const std::string& filename)
{
  // Locks reading from map - maybe a RW lock?
 
  std::lock_guard<std::mutex> lock((*sessions_mutex));
  
  if ((*fileToSession).find(filename) == (*fileToSession).end())
  {
    Write(client, "Rejected");
    return;
  }
  
  sessionIdx = (*fileToSession)[filename];
  if (!JoinSession(sessionIdx))
  {
    Write(client, "Rejected");
    return;
  }

  Write(client, "Succes");
}

bool HandlerThread::JoinSession(int sessIdx)
{
  // Write Session lock
 
  std::lock_guard<std::mutex> lock((*sessions)[sessIdx]->content_mutex);
  
  auto& tmpClients = (*sessions)[sessIdx]->clients;
  
  if (tmpClients.size() == SESSION_CLIENTS_MAX)
  {
    sessionIdx = -1;
    return false;
  }
  
  tmpClients.push_back(client);
  return true;
}

void HandlerThread::InitSession()
{
  std::lock_guard<std::mutex> lock(*sessions_mutex);
  
  Session* tmpSession = new Session();
  tmpSession->filename = currentFilename;
  (*fileToSession)[currentFilename] = (*sessions).size();
  (*sessions).push_back(tmpSession); 
}

bool HandlerThread::ExitSession(int sessIdx)
{
  // Write Session lock
 
  std::lock_guard<std::mutex> lock((*sessions)[sessIdx]->content_mutex);
  
  auto& tmpClients = (*sessions)[sessIdx]->clients;
  std::vector<int>::iterator it;
  
  it = std::find(tmpClients.begin(), tmpClients.end(), client);

  tmpClients.erase(it);
  return true;
}
