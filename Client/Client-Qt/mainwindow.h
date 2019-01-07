#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QPlainTextEdit>
#include <QDebug>

#include "requestshandler.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

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
    void ApplyUpdate(const QString &plaintext);

private slots:
    void SendClosingOperation();
    void SendUpdateOnContentChange(int position,
                                   int charsRemoved,
                                   int charsAdded);
    void SeekUpdates();

private:
    Ui::MainWindow *ui;
    RequestsHandler *handler;
    QPlainTextEdit* editor;
    QTimer* timer;
    bool externUpdate;
};


class EditQDockWidget : public QDockWidget {

    Q_OBJECT

signals:
    void onClosing();

public:
    using QDockWidget::QDockWidget;
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
