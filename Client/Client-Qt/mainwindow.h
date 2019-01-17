#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QPlainTextEdit>
#include <QDebug>

#include "requestshandler.h"
#include "editqdockwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void ServerDown();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_createFile_clicked();

    void on_downloadFile_clicked();

    void on_editFile_clicked();

private:

    void SaveFileAs(const QString &data, const QString &filename);
    void WriteDataToFile(const QString &data, const QString &path);
    void RefreshDataTable();
    void ShowError(QString err);
    void ApplyUpdate(const QString &plaintext, int cursorPos);

private slots:
    void OnRefreshDataTable();
    void SendClosingOperation();
    void SendUpdateOnContentChange(int position,
                                   int charsRemoved,
                                   QString charsAdded);
    void SeekUpdates();
    void SendUpdateCursorChange(int);

private:
    Ui::MainWindow *ui;
    RequestsHandler *handler;
    QPlainTextEdit* editor;
    EditQDockWidget* dock;
    QTimer* timer;
    QTimer* timerTable;
    bool externUpdate;
    bool started;
};

#endif // MAINWINDOW_H
