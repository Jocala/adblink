#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>

class QTextBrowser;
class QPushButton;

class helpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit helpDialog(QWidget *parent = nullptr);
    ~helpDialog();

private:
    QTextBrowser *m_textBrowser;
    QPushButton *m_pushButton;
};

#endif // HELPDIALOG_H
