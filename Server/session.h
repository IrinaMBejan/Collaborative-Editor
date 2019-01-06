#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <mutex>

#define SESSION_CLIENTS_MAX 2

struct Session
{
  std::string filename;
  std::string content;
  std::vector<int> clients;
  std::mutex content_mutex;
};

#endif // SESSION_H
