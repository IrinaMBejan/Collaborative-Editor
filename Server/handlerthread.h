#ifndef HANDLERTHREAD_H
#define HANDLERTHREAD_H

#include <string>

#define FILE_LIST "list.csv"

class HandlerThread
{

public:
  HandlerThread(int cl): client(cl){}
  void Start();

private:
  void HandleMessage(const std::string& msg);
  void HandleRetrieveRequest();
  void HandleCreateFileRequest(const std::string& filename);
  void HandleDownloadRequest(const std::string& filename);
  void HandleEditRequest(const std::string& filename);

  
  int client;
};

#endif // HANDLERTHREAD_H
