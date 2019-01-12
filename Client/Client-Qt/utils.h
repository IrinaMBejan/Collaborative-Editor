#ifndef UTILS_H
#define UTILS_H

#include <errno.h>
#include <string>
#include <QMessageBox>

#define CHECK_ERROR(operation, error)             \
({                                                \
  if (operation == -1)                            \
  {                                               \
    ShowNetworkError("A network error occured!"); \
    perror(error);                                \
    errno;                                        \
  }                                               \
})

#define ERROR(err)                    \
({                                    \
  perror(err);                        \
  errno;                              \
})

// Reads from descripter d the number of bytes sent through the channel,
// allocates the memory and read it.
int Read(int d, std::string& buffer);
void ReadNumber(int d, int number);

// Writes to given descriptor the number of bytes to be written and then
// the data itself.
void Write(int d, const std::string& data);

void ShowNetworkError(const char* err);
#endif // UTILS_H
