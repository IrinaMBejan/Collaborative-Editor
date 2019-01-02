#include <regex>

#include "server.h"
#include "utils.h"
#include "csv.h"

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


static bool UserExists(const std::string& user, const std::string& pass)
{
  CSVReader reader(LOGIN_FILE);

  const std::vector<CSVRow> users = reader.GetData();
  for(int idx = 0; idx < users.size(); idx++)
  {
    if (users[idx].GetAt(0) == user && users[idx].GetAt(1) == pass)
      return true;
  }

  return false;
}

static bool TryLogin(int d)
{
  std::string buffer;
  Read(d, buffer);

  std::regex regex("login ([a-zA-Z]*) ([a-zA-Z_0-9]*)");
  std::smatch matches;
  if (std::regex_match(buffer, matches, regex))
  {
    if (matches.size() == 3)
    {
      std::string user = matches[1].str();
      std::string pass = matches[2].str();
      if (UserExists(user, pass))
      {
        printf("Login succesful for user %s", matches[1].str());
        Write(d, "Succes");
        return true;
      }
    }
  }

  Write(d,"Reject");
  return false;
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
    
    if (TryLogin(client))
    {
      // TODO: instantiate handler thread 
    } 
  }
}


