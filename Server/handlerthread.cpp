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
static constexpr const char* folder = "./files/";

void HandlerThread::Start()
{
  std::string buffer;

  while(true)
  {
    Read(client, buffer);

    HandleMessage(buffer);
    buffer.clear();
  }
}

void HandlerThread::HandleMessage(const std::string& buffer)
{
  std::smatch matches;

  std::regex retrieve_list_reg(retrieve_list);
  std::regex download_file_reg(download_file);
  std::regex create_file_reg(create_file);
  std::regex edit_file_reg(edit_file);

  if (std::regex_match(buffer, retrieve_list_reg))
  {
    HandleRetrieveRequest();
  }
  else if (std::regex_match(buffer, create_file_reg))
  {
    if (matches.size() == 2)
    {
      std::string filename = matches[1].str();
      HandleCreateFileRequest(filename);
    }
  }
  else if (std::regex_match(buffer, download_file_reg))
  {
    if (matches.size() == 2)
    {
      std::string filename = matches[1].str();
      HandleDownloadRequest(filename);
    }
  }
  else if (std::regex_match(buffer, edit_file_reg))
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
  }
}

void HandlerThread::HandleRetrieveRequest()
{
  std::string filepath = folder;
  filepath += FILE_LIST;
  CSVReader reader(filepath);
  const std::vector<CSVRow> files = reader.GetData();

  std::string res = "";

  for (int idx = 0; idx < files.size(); idx++)
  {
    res += files[idx].GetAt(0);
  }

  printf("Lista este %s", res.c_str());
  Write(client, res);
}

void HandlerThread::HandleCreateFileRequest(const std::string& filename)
{
  std::string filepath = folder;
  filepath += filename;
  CSVWriter writer(filepath);

  Write(client, filepath);
}

void HandlerThread::HandleDownloadRequest(const std::string& filename)
{
  std::string filepath = folder+filename;

  std::ifstream t(filepath, std::ios_base::trunc);
  std::stringstream buffer;
  buffer << t.rdbuf();

  Write(client, buffer.str());
}

void HandlerThread::HandleEditRequest(const std::string& filename)
{
  // TODO : check session & create session
}

