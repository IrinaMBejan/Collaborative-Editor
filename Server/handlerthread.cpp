#include <regex>
#include <fstream>

#include "handlerthread.h"
#include "utils.h"
#include "csv.h"


static constexpr const char* retrieve_list = "retrieve files";
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

  while(1)
  {
    if (Read(client, buffer))
    {
      HandleMessage(buffer);
      buffer.clear();
    }
  }
}

void HandlerThread::HandleMessage(const std::string& buffer)
{
  if (!buffer.size())
    return;

  std::smatch matches;

  std::regex retrieve_list_reg(retrieve_list);
  std::regex download_file_reg(download_file);
  std::regex create_file_reg(create_file);
  std::regex edit_file_reg(edit_file);

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
  else 
  {
    printf("Don't know how to treat this: %s\n", buffer.c_str());
    Write(client, "Rejected");
  }
}

void HandlerThread::HandleRetrieveRequest()
{
  std::string filepath = folder;
  filepath += FILE_LIST;
  
  CSVReader reader(filepath);
  const std::vector<CSVRow> files = reader.GetData();

  std::string res = "";
  for (int idx = 0; idx < files.size(); ++idx)
  {
    res += files[idx].GetAt(0);
    res += ",";
    res += files[idx].GetAt(1);

    if (idx +1 < files.size())
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
  
  Write(client, "Succes");
  Write(client, buffer.str());
}

void HandlerThread::HandleEditRequest(const std::string& filename)
{
  Write(client, "Succes");
  // TODO : check session & create session
}

