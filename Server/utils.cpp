#include <cstdio>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils.h"

int Read(int d, char** buffer)
{
    int len;

    CHECK_ERROR((read(d, &len, sizeof(int))),"Read no. of bytes");

    void *memory = malloc(len);
    if (memory == NULL) ERROR("Allocation");
    memset(memory, 0, len+1);

    CHECK_ERROR(read(d, memory, len), "Read data");

    *buffer = (char*)memory;
    fflush(stdout);
    return len;
}

void Read(int d, std::string& buffer)
{
  char* tmp_buffer;
  Read(d, &tmp_buffer);
  buffer.assign(tmp_buffer);
}

void Write(int d, const char* data)
{
    int len = strlen(data);

    CHECK_ERROR(write(d, &len, sizeof(int)), "Write no. of bytes");
    CHECK_ERROR(write(d, data, len), "Write data");
}

void Write(int d, const std::string& data)
{
  Write(d, data.c_str());
}


