#include <regex>
#include <thread>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

#include "handlerthread.h"
#include "server.h"
#include "utils.h"
#include "csv.h"

static constexpr const char* folder = 
  "files/";

std::vector<Session*> sessions;
std::mutex sessions_mutex;
std::map<std::string, int> fileToSession;
std::map<std::string, int> sz;

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

  InitSessions();
}

void Server::SetSocketOptions()
{
  int on=1;
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

static std::string LoadData(const std::string& filename)
{
  std::string filepath = folder+filename;
  std::string tmp;  
  std::ifstream t;
  std::stringstream buffer;
  
  t.open(filepath);
  while (!t.eof())
  {
    getline(t, tmp);
    buffer << tmp;
  }
  t.close();

  return buffer.str();
}

void Server::InitSessions()
{
  std::string filepath = folder;
  filepath += FILE_LIST;
  
  CSVReader reader(filepath);
  const std::vector<CSVRow> files = reader.GetData();
  
  for (int idx = 0; idx < files.size(); idx++)
  {
    std::string fname = files[idx].GetAt(0);
    
    Session* tmpSession = new Session();
    tmpSession->filename = fname;
    tmpSession->content = LoadData(fname);
    fileToSession[fname] = sessions.size();
    sessions.push_back(tmpSession);
  }
}

static void SaveSessionOnDisk(Session* sess)
{
  std::lock_guard<std::mutex> guard(sess->content_mutex);

  if (sess->clients.size())
  {
    std::ofstream file;
    file.open("files/"+sess->filename);
    file << sess->content;
    file.close();

  }

  sz[sess->filename]= sess->content.size();
}

static void SaveSessions()
{
  std::lock_guard<std::mutex> guard(sessions_mutex);

  for (auto session: sessions)
  {
    SaveSessionOnDisk(session);
  }
}

static void RefreshList()
{
  std::lock_guard<std::mutex> guard(sessions_mutex);

  std::cout<<"refreshing list"<<std::endl;
  std::map<std::string, int>::iterator it;

  std::string filepath = "files/list.csv";
  CSVWriter listUpdate(filepath);

  for ( it = sz.begin(); it != sz.end(); it++ )
  {
    CSVRow row;
    row.Add(it->first);
    row.Add(std::to_string(it->second));

    std::cout<<"row is "<<it->first <<" "<<it->second<<std::endl;
    it == sz.begin() ? listUpdate.AddRow(row, true) : listUpdate.AddRow(row);
  }
}

static void BackgroundSaveToDisk()
{
  auto start = std::chrono::system_clock::now();

  while(1)
  {
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end-start;

    if (elapsed.count() > 10)
    {
      SaveSessions();
      RefreshList();
      start = std::chrono::system_clock::now(); 
    }
  }
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
        printf("Login succesful for user %s!\n", user.c_str());
        Write(d, "Succes");
        return true;
      }
    }
  }
  printf("Rejected login request.\n");
  Write(d,"Reject");
  return false;
}

static void HandleClient(int cl) 
{
  HandlerThread th(cl, &sessions, &sessions_mutex, &fileToSession);
  th.Start();
}

static void JoinThread(std::thread& t)
{
    t.join();
}

void Server::StartListening()
{
  printf("Waiting for clients on port %d\n", PORT);
  fflush(stdout);

  struct sockaddr_in cl;
  bzero (&cl, sizeof(cl));
  int size = sizeof(cl);
 
  std::vector<std::thread> v;

  threads.push_back(std::thread(BackgroundSaveToDisk));
  
  while (1)
  {
    int client;
    CHECK_ERROR(
        (client = accept(sd, (struct sockaddr*) &cl, (socklen_t*)&size)), 
        "Accept error"); 
    // TODO: this shouldn't crush the server: continue and not return;
    
    if (TryLogin(client))
    {
      printf("Started new thread for client\n");

      threads.push_back(std::thread(HandleClient, client));
    } 
  }

  std::for_each(threads.begin(),threads.end(),JoinThread);
}

