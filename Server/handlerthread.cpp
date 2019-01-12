#include <regex>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>

#include "handlerthread.h"
#include "utils.h"
#include "csv.h"


#define MAX_EMPTY 10

static constexpr const char* retrieve_list = "retrieve files";
static constexpr const char* operations_init = "operations start";
static constexpr const char* operations_close = "operations close";
static constexpr const char* operation_insert = "insert (\\d+) ([\\s\\S]+)";
static constexpr const char* operation_delete = "delete (\\d+) (\\d+)";
static constexpr const char* operation_cursor = "cursor (-?\\d+)";
static constexpr const char* create_file = 
  "create file ([\\w\\d]+.[\\w\\d]+)";
static constexpr const char* download_file = 
  "download file ([\\w\\d]+.[\\w\\d]+)";
static constexpr const char* edit_file = 
  "edit file ([\\w\\d]+.[\\w\\d]+)";
static constexpr const char* folder = 
  "files/";

void HandlerThread::Start()
{
  std::string buffer;
  buffer.clear();
 
  fd_set readfds;
  fd_set actfds;
  struct timeval tv;

  FD_ZERO(&actfds);
  FD_SET(client, &actfds);

  tv.tv_sec = 0;
  tv.tv_usec = 10;

  int nfds = client;

  int tries = 0;
  
  auto start = std::chrono::system_clock::now();

  while (1)
  {
    bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds));

    CHECK_ERROR(select (nfds+1, &readfds, NULL, NULL, &tv), "Select error");

    if (FD_ISSET(client, &readfds))
    {
        if (Read(client, buffer))
        {
            HandleMessage(buffer);
            buffer.clear();
        } 
        else 
        {
            tries += 1;
            if (tries >= MAX_EMPTY)
                break;
        }
    }
    
    if (sessionIdx != -1)
    {
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = end-start;

        if (elapsed.count() > 0.1)
        {
            SendUpdate();
            start = std::chrono::system_clock::now();
        }
    }
  }
}

void HandlerThread::HandleMessage(const std::string& buffer)
{
  std::cout << "in dispatch: " << buffer << std::endl;
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
  std::regex operations_cursor_reg(operation_cursor);

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
  else if (std::regex_match(buffer, matches, operations_cursor_reg))
  {
    if (matches.size() == 2)
    {
      int delta = std::stoi(matches[1].str());
      HandleCursorOperation(delta);
    }
  }
  else 
  {
    printf("Don't know how to treat this: %s\n", buffer.c_str());
    Write(client, "Rejected");
  }
}

void HandlerThread::SendUpdate()
{
    if (sessionIdx != -1)
    {
        std::lock_guard<std::mutex> guard((*sessions)[sessionIdx]->content_mutex);
        Session* session = (*sessions)[sessionIdx];

        Write(client, session->content);
        Write(client, std::to_string(session->cursorPosition[client]));
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
  std::cout << "Closing session: " << sessionIdx << std::endl;
  if (ExitSession(sessionIdx))
  {
    sessionIdx = -1;
    Write(client, "Succes");
  }
}

void HandlerThread::HandleCursorOperation(int delta)
{
  std::lock_guard<std::mutex> guard((*sessions)[sessionIdx]->content_mutex);

  Session* session = (*sessions)[sessionIdx];

  session->cursorPosition[client]+= delta;

  session->cursorPosition[client] = 
      std::min(session->cursorPosition[client], (int)session->content.size());
  session->cursorPosition[client] = 
      std::max(session->cursorPosition[client], 0);
}

void HandlerThread::HandleInsertOperation(int position, std::string text)
{
  std::lock_guard<std::mutex> guard((*sessions)[sessionIdx]->content_mutex);
  
  Session* session = (*sessions)[sessionIdx];
  session->content.insert(position,text);

  for (auto& cursor : session->cursorPosition)
  {
    if (position <= cursor.second)
    {
        cursor.second += text.size();
        cursor.second = std::min(cursor.second, (int)session->content.size());
    }
  }
}

void HandlerThread::HandleDeleteOperation(int position, int length)
{
  std::lock_guard<std::mutex> guard((*sessions)[sessionIdx]->content_mutex);
  
  Session* session = (*sessions)[sessionIdx];
  session->content.erase(position,length);

  for (auto& cursor : session->cursorPosition)
  {
    if (position <= cursor.second && length == 1)
    {
        cursor.second--;
        cursor.second = std::max(cursor.second, 0);
    // TODO: update for longer text
    }
  }

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

static std::string LoadData(const std::string& filename)
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

  return buffer.str();
}

void HandlerThread::HandleDownloadRequest(const std::string& filename)
{
  Write(client, "Succes");
  Write(client, LoadData(filename));
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
 
  Session* session = (*sessions)[sessIdx];
  auto& tmpClients = session->clients;
  
  if (tmpClients.size() == SESSION_CLIENTS_MAX)
  {
    sessionIdx = -1;
    return false;
  }
  
  tmpClients.push_back(client);
  session->cursorPosition[client]=0;
  
  session->content = LoadData(session->filename);

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
  if (sessIdx == -1)
    return true;

  std::lock_guard<std::mutex> lock((*sessions)[sessIdx]->content_mutex);
  
  auto& tmpClients = (*sessions)[sessIdx]->clients;
  std::vector<int>::iterator it;
  
  it = std::find(tmpClients.begin(), tmpClients.end(), client);

  tmpClients.erase(it);
  (*sessions)[sessIdx]->cursorPosition.erase(client);
  return true;
}
