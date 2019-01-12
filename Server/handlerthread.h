#ifndef HANDLERTHREAD_H
#define HANDLERTHREAD_H

#include <map>
#include <string>
#include <vector>
#include "session.h"

#define FILE_LIST "list.csv"

class HandlerThread
{

public:
  HandlerThread(
      int cl, std::vector<Session*> *sess, 
      std::mutex *mutex, 
      std::map<std::string,int> *mapFiles): 
    client(cl), sessionIdx(-1), sessions(sess), 
    sessions_mutex(mutex), fileToSession(mapFiles) {}
  void Start();

private:
  void HandleMessage(const std::string& msg);
  void HandleRetrieveRequest();
  void HandleCreateFileRequest(const std::string& filename);
  void HandleDownloadRequest(const std::string& filename);
  void HandleEditRequest(const std::string& filename);
  void HandleInsertOperation(int position, std::string text);
  void HandleDeleteOperation(int position, int length);
  void HandleOperationStart();
  void HandleOperationClose();

  void SendUpdate();

  bool JoinSession(int sessIdx);
  bool ExitSession(int sessIdx);
  void InitSession();

private:
  int client;
  
  std::string currentFilename;
  int sessionIdx;

  std::vector<Session*> *sessions;
  std::mutex *sessions_mutex;
  std::map<std::string, int> *fileToSession;
};

#endif // HANDLERTHREAD_H
