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
bool Read(int d, std::string& buffer);

// Writes to given descriptor the number of bytes to be written and then
// the data itself.
bool Write(int d, const std::string& data);

bool Connect(int d);

void ShowNetworkError(const char* err);
#endif // UTILS_H
