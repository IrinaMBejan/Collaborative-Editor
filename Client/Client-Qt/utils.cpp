#include <cstdio>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils.h"
#include <QDebug>

int Read(int d, std::string& buffer)
{
    int len = 0;

    CHECK_ERROR((read(d, &len, sizeof(int))),"Read no. of bytes");
    if (!len)
        return 0;

    char *memory = (char*)malloc((len + 1) * sizeof(char));
    if (memory == NULL) ERROR("Allocation");
    memset(memory, 0, len+1);

    CHECK_ERROR(read(d, memory, len), "Read data");

    buffer.assign(memory);
    free(memory);

    qDebug()<<"READ:" << QString::fromStdString(buffer);
    return len;
}

void ReadNumber(int d, int number)
{
    CHECK_ERROR((read(d, &number, sizeof(int))),"Read no. of bytes");
}

static void Write(int d, const char* data)
{
    int len = strlen(data);

    CHECK_ERROR(write(d, &len, sizeof(int)), "Write no. of bytes");
    CHECK_ERROR(write(d, data, len), "Write data");

    qDebug()<<"WRITE:" << QString::fromStdString(data);

}

void Write(int d, const std::string& data)
{
  Write(d, data.c_str());
}

void ShowNetworkError(const char* err)
{
    QMessageBox msgBox;
    msgBox.setText(err);
    msgBox.exec();
}
