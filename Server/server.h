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

#define PORT 2991 
#define LOGIN_FILE "./files/users.csv" 

class Server 
{

public:
  Server();
  void StartListening();

private:
  void SetSocketOptions();
  int sd; //socket descriptor
};

#endif // SERVER_H
