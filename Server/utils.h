#include <errno.h>
#include <cstdio>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
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

int Read(int d, std::string& buffer)
{
  // TODO :implement
}

// Reads from descripter d the number of bytes sent through the channel,
// allocates the memory and read it. It returns the number of bytes read.
int Read(int d, char** buffer)
{
    int len;

    CHECK_ERROR((read(d, &len, sizeof(int))),"Read no. of bytes");

    void *memory = malloc(len);
    if (memory == NULL) ERROR("Allocation");
    memset(memory, 0, len+1);

    CHECK_ERROR(read(d, memory, len), "Read data");

    *buffer = (char*)memory;
    return len;
}

void Write(int d, const std::string& data)
{
  Write(d, data.c_str());
}

// Writes to given descriptor the number of bytes to be written and then
// the data itself.
void Write(int d, const char* data)
{
    int len = strlen(data);

    CHECK_ERROR(write(d, &len, sizeof(int)), "Write no. of bytes");
    CHECK_ERROR(write(d, data, len), "Write data");
}
