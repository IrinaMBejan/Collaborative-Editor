#include <regex>

#include "handlerthread.h"
#include "utils.h"

static constexpr const char* retrieve_list = "retrieve files";
static constexpr const char* create_fle = 
  "create file ([a-zA-Z0-9]+.[a-zA-Z0-9])";
static constexpr const char* download_file = 
  "download file ([a-zA-Z0-9]+.[a-zA-Z0-9])";

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

void HandlerThread::HandleMessage(const std::string& msg)
{

}

void HandlerThread::HandleRetrieveRequest(const std::string& req)
{
  std::regex retrieve_list_reg(retrieve_list);
}

void HandlerThread::HandleCreateFileRequest(const std::string& req)
{
  std::regex create_file_reg(create_fle);
}

void HandlerThread::HandleDownloadRequest(const std::string& req)
{
  std::regex download_file_reg(download_file);
}

void HandlerThread::HandleEditRequest(const std::string& req)
{

}

