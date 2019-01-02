#include "server.h"
#include "utils.h"

Server::Server()
{
  CHECK_ERROR((sd = socket(AF_INET, SOCK_STREAM,0)), "Socket error");
 
  SetSocketOptions();

  struct sockaddr_in server;
  bzero (&server, sizeof(server));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(PORT);

  CHECK_ERROR(bind(sd, (struct sockaddr*) &server, sizeof(struct sockaddr)), 
      "Bind error");

  CHECK_ERROR(listen(sd,2),"Listen error");

}

void Server::SetSocketOptions()
{
  int on=1;
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

void Server::StartListening()
{
  printf("Waiting for clients on port %d\n", PORT);
  fflush(stdout);

  struct sockaddr_in cl;
  bzero (&cl, sizeof(cl));
  int size = sizeof(cl);
  
  while (1)
  {
    int client;
    CHECK_ERROR(
        (client = accept(sd, (struct sockaddr*) &cl, (socklen_t*)&size)), 
        "Accept error"); 
    // TODO: this shouldn't crush the server: continue and not return;
  }
}
