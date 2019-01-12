#ifndef REQUESTSHANDLER_H
#define REQUESTSHANDLER_H
#include <QString>
#include <vector>

class RequestsHandler
{
public:
    RequestsHandler();

    bool SendLoginRequest(const std::string& user, const std::string& pass);
    bool SendRetrieveFilesRequest(QStringList& list);
    bool SendDownloadFileRequest(QString filename, QString& data);
    bool SendCreateFileRequest(std::string filename);
    bool SendEditRequest(std::string filename);

    bool SendOperationInit();
    bool SendOperationClose();

    bool SendDeleteOperation(int position, int count);
    bool SendInsertOperation(int position, const std::string& text);

    std::string FetchUpdates();

private:

    QStringList ExtractListOfFiles(QString data);

private:
    int sd;
};

#endif // REQUESTSHANDLER_H
