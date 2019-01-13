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

bool RequestsHandler::TryConnect()
{
    if ((sd = socket(AF_INET, SOCK_STREAM,0)) < 0)
        return false;

    return Connect(sd);
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
        if (!WriteS(sd,request))
            return false;

        std::string response;
        if (!ReadS(sd,response))
            return false;

        return (response == "Succes");
    }

    return false;
}

bool RequestsHandler::SendRetrieveFilesRequest(QStringList& list)
{
    std::string request = "retrieve files";
    if (!WriteS(sd, request))
        return false;

    std::string response;

    if (!ReadS(sd,response))
        return false;

    if (response == "Succes")
    {
        std::string data;

        if (!ReadS(sd, data))
            return false;

        list = ExtractListOfFiles(QString::fromStdString(data));
    }
    return (response == "Succes");
}

bool RequestsHandler::SendDownloadFileRequest(QString filename, QString& result)
{
    std::string request = "download file " + filename.toStdString();
    if (!WriteS(sd, request))
        return false;

    std::string response;
    if (!ReadS(sd,response))
        return false;

    if (response == "Succes")
    {
        std::string data;

        if (!ReadS(sd, data))
            return false;

        result = QString::fromStdString(data);
    }
    return (response == "Succes");
}

bool RequestsHandler::SendCreateFileRequest(std::string filename)
{
    std::string request = "create file " + filename;
    if (!WriteS(sd, request))
        return false;

    std::string response;
    if (!ReadS(sd,response))
        return false;

    return (response == "Succes");
}

bool RequestsHandler::SendEditRequest(std::string filename)
{
    std::string request = "edit file " + filename;
    if (!WriteS(sd, request))
        return false;

    std::string response;
    if (!ReadS(sd,response))
        return false;

    return (response == "Succes");
}

bool RequestsHandler::SendOperationInit()
{
    std::string request = "operations start";
    if (!WriteS(sd, request))
        return false;

    std::string response;
    if (!ReadS(sd,response))
        return false;

    return (response == "Succes");
}

bool RequestsHandler::SendOperationClose()
{
    std::string request = "operations close";
    if (!WriteS(sd, request))
        return false;

    std::string response;
    if (!ReadS(sd,response))
        return false;

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

    return WriteS(sd, request);
}

bool RequestsHandler::SendInsertOperation(int position, const std::string &text)
{
    std::string request = "insert " +
            std::to_string(position) +
            " " +
            text;

    return WriteS(sd, request);
}

bool RequestsHandler::SendCursorOperation(int diff)
{
    std::string request = "cursor " +
            std::to_string(diff);

    return WriteS(sd, request);
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
        if (!ReadS(sd, tmpBuff))
            return false;
        if (!ReadS(sd, cursorBuff))
            return false;

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

bool RequestsHandler::ReadS(int d, std::string &buffer)
{
    if (!Read(d,buffer))
    {
        emit notifyServerDown();
        return false;
    }

    return true;
}

bool RequestsHandler::WriteS(int d, const std::string &data)
{
    if (!Write(d, data))
    {
        emit notifyServerDown();
        return false;
    }

    return true;
}

QStringList RequestsHandler::ExtractListOfFiles(
        QString data)
{
    return data.split(QString(","),QString::SplitBehavior::SkipEmptyParts);
}
