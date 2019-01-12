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
    void ApplyUpdate(const QString &plaintext, int cursorPos);

private slots:
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
    QTimer* timer;
    bool externUpdate;
};


class EditQDockWidget : public QDockWidget {

    Q_OBJECT

signals:
    void onClosing();

public:
    using QDockWidget::QDockWidget;
    void closeEvent(QCloseEvent *) override;
};

class QPlainTextEditView: public QPlainTextEdit {
    Q_OBJECT

public:
    using QPlainTextEdit::QPlainTextEdit;
    void keyPressEvent(QKeyEvent *e) override;

signals:
    void contentsChange(int, int, QString);
    void cursorChange(int);
};

#endif // MAINWINDOW_H
