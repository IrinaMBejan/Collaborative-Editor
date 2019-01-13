#ifndef REQUESTSHANDLER_H
#define REQUESTSHANDLER_H
#include <QString>
#include <QObject>
#include <vector>

class RequestsHandler: public QObject
{
    Q_OBJECT

public:
    explicit RequestsHandler(QObject *parent = nullptr): QObject(parent) {}

    bool TryConnect();
    bool SendLoginRequest(const std::string& user, const std::string& pass);
    bool SendRetrieveFilesRequest(QStringList& list);
    bool SendDownloadFileRequest(QString filename, QString& data);
    bool SendCreateFileRequest(std::string filename);
    bool SendEditRequest(std::string filename);

    bool SendOperationInit();
    bool SendOperationClose();

    bool SendDeleteOperation(int position, int count);
    bool SendInsertOperation(int position, const std::string& text);
    bool SendCursorOperation(int diff);

    bool FetchUpdates(QString& text, int& pos);
    bool ReadS(int d, std::string& buffer);
    bool WriteS(int d, const std::string& data);

signals:
    void notifyServerDown();

private:

    QStringList ExtractListOfFiles(QString data);

private:
    int sd;
};

#endif // REQUESTSHANDLER_H
