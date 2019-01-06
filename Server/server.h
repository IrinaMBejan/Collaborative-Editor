#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <mutex>
#include <vector>
#include <map>

#include "session.h"

#define PORT 2985 
#define LOGIN_FILE "./files/users.csv" 

extern std::vector<Session*> sessions;
extern std::mutex sessions_mutex;
extern std::map<std::string, int> fileToSession;

class Server 
{

public:
  Server();
  void StartListening();

private:
  void InitSessions();
  void SetSocketOptions();
  int sd; //socket descriptor
  
};

#endif // SERVER_H
