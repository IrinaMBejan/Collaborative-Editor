#include <regex>
#include "handlerthread.h"
#include "utils.h"

constexpr char* filename_reg = "([a-zA-Z0-9]+.[a-zA-Z0-9])";
const std::regex retrieve_list_reg("retrieve files");
const std::regex create_file_reg(strcat("create file ",filename_reg));
const std::regex download_file_reg(strcat("download file ", filename_reg));


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
}

void HandlerThread::HandleCreateFileRequest(const std::string& req)
{}

void HandlerThread::HandleDownloadRequest(const std::string& req)
{}

void HandlerThread::HandleEditRequest(const std::string& req)
{}

