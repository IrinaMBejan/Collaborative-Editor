#include <string>

class HandlerThread
{

public:
  HandlerThread(int cl): client(cl){}
  void Start();

private:
  void HandleMessage(std::string msg);
  
  int client;
};
