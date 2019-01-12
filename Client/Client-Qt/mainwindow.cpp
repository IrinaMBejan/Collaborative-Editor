#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QtDebug>
#include <QFileDialog>
#include <QDockWidget>
#include <QPlainTextEdit>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    handler = new RequestsHandler();

    ui->setupUi(this);
    ui->Filelist->setVisible(0);
    for (int idx = 0; idx < ui->dataTable->horizontalHeader()->count(); ++idx)
    {
        ui->dataTable->horizontalHeader()->setSectionResizeMode(
            idx, QHeaderView::Stretch);
    }

    externUpdate = false;

    emit on_pushButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    std::string user = ui->userField->text().toStdString();
    std::string pass = ui->passField->text().toStdString();

    user = "ana";
    pass = "pass123";

    if (!handler->SendLoginRequest(user, pass))
    {
        ShowError("The login failed, try again with different credentials.");
        return;
    }

    RefreshDataTable();
}

void MainWindow::on_createFile_clicked()
{
    std::string filename = ui->newFileName->text().toStdString();

    if (!filename.size())
    {
        ShowError("Must set a name for the file in the text box !");
        return;
    }

    if (filename.find(".") == std::string::npos ||
        filename[0] == '.' ||
        filename[filename.size()-1] == '.')
    {
        ShowError("Please select a valid filename !");
        return;
    }

    if (!handler->SendCreateFileRequest(filename))
    {
        ShowError("The creation of file failed. Try again maybe ? :) ");
        return;
    }

    RefreshDataTable();
}

void MainWindow::on_downloadFile_clicked()
{
    if (ui->dataTable->selectedItems().size() == 0)
    {
        ShowError("You have to select a file or more!");
        return;
    }

    for (int idx = 0; idx < ui->dataTable->selectedItems().size(); idx++)
    {
        if (ui->dataTable->selectedItems()[idx]->column() == 0)
        {
            QString data;
            QString filename = ui->dataTable->selectedItems()[idx]->text();
            if (!handler->SendDownloadFileRequest(filename, data))
            {
                ShowError("Ooops! Download failed!");
                return;
            }
            else
            {
                SaveFileAs(data, filename);
            }
        }
    }
}

void MainWindow::ShowError(QString err)
{
    QMessageBox msgBox;
    msgBox.setText(err);
    msgBox.exec();
}

void MainWindow::on_editFile_clicked()
{
    if (ui->dataTable->selectedItems().size() == 0)
    {
        ShowError("You have to select a file or more!");
        return;
    }

    QString filename = ui->dataTable->selectedItems()[0]->text();

    if (!handler->SendEditRequest(filename.toStdString()))
    {
        ShowError("Session is full for this file!");
        return;
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(SeekUpdates()));
    timer->start(500);

    EditQDockWidget* dock = new EditQDockWidget(filename, this);
    dock->setFloating(true);

    editor = new QPlainTextEditView(dock);
    QObject::connect(editor, SIGNAL(contentsChange(int,int,QString)),
                     this, SLOT(SendUpdateOnContentChange(int,int,QString)));
    QObject::connect(editor, SIGNAL(cursorChange(int)),
                     this, SLOT(SendUpdateCursorChange(int)));
    QObject::connect(dock, SIGNAL(onClosing()),
                     this, SLOT(SendClosingOperation()));

    dock->setWidget(editor);

    dock->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    dock->setMinimumSize(QSize(600,800));

    dock->show();
}

void MainWindow::SendClosingOperation()
{
    timer->stop();
    SeekUpdates();

    qDebug() << "Closing operation...";
    if (!handler->SendOperationClose())
    {
        ShowError("Something went wrong");
        return;
    }
}

void MainWindow::SendUpdateCursorChange(int diff)
{
    if (!diff)
        return;

    handler->SendCursorOperation(diff);
}

void MainWindow::SendUpdateOnContentChange(
        int position, int charsRemoved, QString charsAdded)
{
//    if (externUpdate)
//    {
//        return;
//    }

    if (charsRemoved)
    {
        handler->SendDeleteOperation(position + charsRemoved - 1,
                                     charsRemoved);
    }

    if (!charsAdded.isEmpty())
    {
        handler->SendInsertOperation(
                    position,
                    charsAdded.toStdString());
    }

}

void MainWindow::SeekUpdates()
{
    externUpdate = true;
    QString str;
    int cursorPos;

    str.clear();

    handler->FetchUpdates(str, cursorPos);

    if (!str.isEmpty())
    {
        qDebug() << "applying updates";
        ApplyUpdate(str, cursorPos);
    }

    externUpdate = false;
}

void MainWindow::ApplyUpdate(const QString &plaintext, int cursorPos)
{
    externUpdate = true;

    editor->setPlainText(plaintext);

    QTextCursor oldCursor = editor->textCursor();
    oldCursor.setPosition(cursorPos);
    editor->setTextCursor(oldCursor);

    externUpdate = false;
}

void MainWindow::SaveFileAs(const QString& data, const QString& filename)
{
    auto saveAsDialog(new QFileDialog(this));
    saveAsDialog->selectFile(filename);
    saveAsDialog->setWindowModality( Qt::WindowModal );
    saveAsDialog->setFileMode( QFileDialog::AnyFile );
    saveAsDialog->setDefaultSuffix(".txt");
    saveAsDialog->setAcceptMode( QFileDialog::AcceptSave );
    saveAsDialog->setDirectory("/home");

    QStringList fileNames;
    if (saveAsDialog->exec())
        fileNames = saveAsDialog->selectedFiles();

    if (fileNames[0] != "")
    {
        WriteDataToFile(data, fileNames[0]);
    }
}

void MainWindow::WriteDataToFile(const QString& data, const QString& path)
{
    QFile file(QFileInfo(path).absoluteFilePath());
    qDebug() << data;
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);
        out << data;
        file.close();
    }
    else
    {
        ShowError("Couldn't save the file. Sorry :( !");
        return;
    }
}

void MainWindow::RefreshDataTable()
{
    QStringList list;

    if (handler->SendRetrieveFilesRequest(list))
    {
        ui->Loginform->setVisible(0);
        ui->Filelist->setVisible(1);

        ui->dataTable->setRowCount(0);

        for(int idx = 0; idx < list.size(); idx+=2)
        {
            ui->dataTable->insertRow( ui->dataTable->rowCount() );
            ui->dataTable->setItem  ( ui->dataTable->rowCount()-1, 0,
                                      new QTableWidgetItem(list.at(idx)));
            ui->dataTable->setItem  ( ui->dataTable->rowCount()-1, 1,
                                     new QTableWidgetItem(list.at(idx+1)));
        }
    }
    else
    {
        ShowError("An error occured, please restart the application.");
    }
}


void EditQDockWidget::closeEvent(QCloseEvent *event)
{
    emit onClosing();
    event->accept();
}

void QPlainTextEditView::keyPressEvent(QKeyEvent *e)
{
    qDebug() << "Avem o tasta"<< e->key();

    switch (e->key())
    {
        case Qt::Key_Backspace:
            emit contentsChange(textCursor().position()-1, 1, "");
            break;
        case Qt::Key_Delete:
            emit contentsChange(textCursor().position()-1, 1, "");
            break;
        case Qt::Key_Left:
            emit cursorChange(-1);
            break;
        case Qt::Key_Right:
            emit cursorChange(1);
            break;
        default:
            emit contentsChange(textCursor().position(), 0, e->text());
            break;
    }

    e->ignore();
}
