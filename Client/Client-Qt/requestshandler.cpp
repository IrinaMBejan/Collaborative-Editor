#include "requestshandler.h"
#include "utils.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <QDebug>
#define PORT 2991
#define SERVER_ADDRESS "127.0.0.1"

RequestsHandler::RequestsHandler()
{
    struct sockaddr_in server;

    CHECK_ERROR((sd = socket(AF_INET, SOCK_STREAM,0)), "Socket error");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server.sin_port = htons(PORT);

    CHECK_ERROR(connect(sd, (struct sockaddr *) &server,
                        sizeof(struct sockaddr)), "Connect error");
}

bool RequestsHandler::SendLoginRequest(const std::string& user,
                                       const std::string& pass)
{
    std::string request = "login ";
    request += user;
    request += ' ';
    request += pass;

    if (!user.empty() && !pass.empty())
    {
        Write(sd,request);

        std::string response;
        Read(sd,response);

        return (response == "Succes");
    }

    return false;
}

bool RequestsHandler::SendRetrieveFilesRequest(QStringList& list)
{
    std::string request = "retrieve files";
    Write(sd, request);

    std::string response;
    Read(sd,response);

    if (response == "Succes")
    {
        std::string data;

        Read(sd, data);
        list = ExtractListOfFiles(QString::fromStdString(data));
    }
    return (response == "Succes");
}

bool RequestsHandler::SendDownloadFileRequest(QString filename, QString& result)
{
    std::string request = "download file " + filename.toStdString();
    Write(sd, request);

    std::string response;
    Read(sd,response);

    if (response == "Succes")
    {
        std::string data;

        Read(sd, data);
        result = QString::fromStdString(data);
    }
    return (response == "Succes");
}

bool RequestsHandler::SendCreateFileRequest(std::string filename)
{
    std::string request = "create file " + filename;
    Write(sd, request);

    std::string response;
    Read(sd,response);

    return (response == "Succes");
}

bool RequestsHandler::SendEditRequest(std::string filename)
{
    std::string request = "edit file " + filename;
    Write(sd, request);

    std::string response;
    Read(sd,response);

    return (response == "Succes");
}

QStringList RequestsHandler::ExtractListOfFiles(
        QString data)
{
    return data.split(QString(","),QString::SplitBehavior::SkipEmptyParts);
}
