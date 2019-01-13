#ifndef EDITQDOCKWIDGET_H
#define EDITQDOCKWIDGET_H

#include <QDockWidget>
#include <QPlainTextEdit>

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
    void mousePressEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent* event) override;

signals:
    void contentsChange(int, int, QString);
    void cursorChange(int);
};


#endif // EDITQDOCKWIDGET_H
