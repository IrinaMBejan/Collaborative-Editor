#include "editqdockwidget.h"

#include <QDebug>

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
