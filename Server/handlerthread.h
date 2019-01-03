#ifndef HANDLERTHREAD_H
#define HANDLERTHREAD_H

#include <string>

class HandlerThread
{

public:
  HandlerThread(int cl): client(cl){}
  void Start();

private:
  void HandleMessage(const std::string& msg);
  void HandleRetrieveRequest(const std::string& req);
  void HandleCreateFileRequest(const std::string& req);
  void HandleDownloadRequest(const std::string& req);
  void HandleEditRequest(const std::string& req);

  
  int client;
};

#endif // HANDLERTHREAD_H
