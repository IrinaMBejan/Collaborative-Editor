#include "requestshandler.h"
#include "utils.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <regex>

#include <QDebug>
#define PORT 2985
#define SERVER_ADDRESS "127.0.0.1"

RequestsHandler::RequestsHandler()
{
}

bool RequestsHandler::TryConnect()
{
    struct sockaddr_in server;

    if ((sd = socket(AF_INET, SOCK_STREAM,0)) < 0)
        return false;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server.sin_port = htons(PORT);

    if (connect(sd, (struct sockaddr *) &server,
                        sizeof(struct sockaddr)) < 0)
        return false;

    return true;
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

bool RequestsHandler::SendOperationInit()
{
    std::string request = "operations start";
    Write(sd, request);

    std::string response;
    Read(sd,response);

    return (response == "Succes");
}

bool RequestsHandler::SendOperationClose()
{
    std::string request = "operations close";
    Write(sd, request);

    std::string response;
    Read(sd,response);

    return (response == "Succes");
}

bool RequestsHandler::SendDeleteOperation(int position, int count)
{
    if (position < 0)
        return true;

    std::string request = "delete " +
            std::to_string(position) +
            " " +
            std::to_string(count);

    Write(sd, request);

    return true;
}

bool RequestsHandler::SendInsertOperation(int position, const std::string &text)
{
    std::string request = "insert " +
            std::to_string(position) +
            " " +
            text;

    Write(sd, request);

    return true;
}

bool RequestsHandler::SendCursorOperation(int diff)
{
    std::string request = "cursor " +
            std::to_string(diff);

    Write(sd, request);

    return true;
}

bool RequestsHandler::FetchUpdates(QString& text, int& pos)
{
    std::string tmpBuff;
    std::string cursorBuff;

    fd_set readfds;
    fd_set actfds;
    struct timeval tv;

    FD_ZERO(&actfds);
    FD_SET(sd, &actfds);

    tv.tv_sec = 0;
    tv.tv_usec = 1;

    int nfds = sd;

    std::regex plaintext("text ([\\s\\S]*)");
    std::regex cursor("cursor (\\d+)");
    std::smatch matches;


    bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds));

    CHECK_ERROR(select (nfds+1, &readfds, NULL, NULL, &tv), "Select error");

    if (FD_ISSET(sd, &readfds))
    {
        tmpBuff.clear();
        cursorBuff.clear();
        Read(sd, tmpBuff);
        Read(sd, cursorBuff);

        int match = 0;
        if (std::regex_match(tmpBuff, matches, plaintext) &&
                matches.size() == 2)
        {
            text = QString::fromStdString(matches[1].str());
            match++;
        }

        if (std::regex_match(cursorBuff, matches, cursor) &&
                matches.size() == 2)
        {
            pos = std::stoi(matches[1].str());
            match++;
        }
        if (match == 2)
        {
            qDebug() << "match\n";

            return true;
        }
    }
    return false;
}

QStringList RequestsHandler::ExtractListOfFiles(
        QString data)
{
    return data.split(QString(","),QString::SplitBehavior::SkipEmptyParts);
}
