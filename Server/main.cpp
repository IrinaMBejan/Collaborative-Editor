#include <string>
#include <vector>
#include <memory>
#include "server.h"

int main()
{
  Server server;
  server.StartListening();
  return 0;
}
