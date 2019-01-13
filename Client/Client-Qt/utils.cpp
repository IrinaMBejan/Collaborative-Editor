#include <cstdio>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "utils.h"
#include <QDebug>


#define PORT 2985
#define SERVER_ADDRESS "127.0.0.1"

bool Read(int d, std::string& buffer)
{
    int len = 0;

    if (read(d, &len, sizeof(int))<0)
    {
        return false;
    }

    if (!len)
        return false;

    char *memory = (char*)malloc((len + 1) * sizeof(char));
    if (memory == NULL) ERROR("Allocation");
    memset(memory, 0, len+1);

    if(read(d, memory, len) < 0)
    {
        return false;
    }

    buffer.assign(memory);
    free(memory);

    qDebug()<<"READ:" << QString::fromStdString(buffer);
    return len;
}

static bool Write(int d, const char* data)
{
    int len = strlen(data);

    if (write(d, &len, sizeof(int)) <0) return false;
    if (write(d, data, len) <0) return false;

    qDebug()<<"WRITE:" << QString::fromStdString(data);

    return true;
}

bool Write(int d, const std::string& data)
{
  return Write(d, data.c_str());
}

void ShowNetworkError(const char* err)
{
    QMessageBox msgBox;
    msgBox.setText(err);
    msgBox.exec();
}

bool Connect(int d)
{
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server.sin_port = htons(PORT);

    return (connect(d, (struct sockaddr *) &server,
                            sizeof(struct sockaddr)) != -1);
}
