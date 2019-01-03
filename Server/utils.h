#include <errno.h>
#include <string>

#define CHECK_ERROR(operation, error) \
({                                    \
  if (operation == -1)                \
  {                                   \
    perror(error);                    \
    errno;                            \
  }                                   \
})                                    

#define ERROR(err)                    \
({                                    \
  perror(err);                        \
  errno;                              \
})

// Reads from descripter d the number of bytes sent through the channel,
// allocates the memory and read it. 
int Read(int d, std::string& buffer);

// Writes to given descriptor the number of bytes to be written and then
// the data itself.
void Write(int d, const std::string& data);
