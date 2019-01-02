#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#define PORT 2996 

class Server 
{

public:
  Server();
  void StartListening();

private:
  void SetSocketOptions();

  int sd; //socket descriptor
};
