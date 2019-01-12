#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <mutex>
#include <map>
#include <vector>

#define SESSION_CLIENTS_MAX 2

struct Session
{
  std::string filename;
  std::string content;
  std::vector<int> clients;
  std::mutex content_mutex;
  std::map<int, int> cursorPosition;
};

#endif // SESSION_H
