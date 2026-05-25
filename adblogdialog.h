#ifndef ADBLOGDIALOG_H
#define ADBLOGDIALOG_H

#include <QDialog>
#include <QClipboard>

class QLabel;
class QTextBrowser;
class QPushButton;

class adblogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit adblogDialog(QWidget *parent = 0);
    ~adblogDialog();

private slots:
    void on_copyButton_clicked();
    void on_swapButton_clicked();

private:
    QLabel *logfileName;
    QTextBrowser *logBrowser;
    QPushButton *copyButton;
    QPushButton *swapButton;
    QPushButton *quitButton;
};

extern int os1;
extern QString logdir;
extern QString logfile1;
extern QString logfile2;
extern QString content;
extern bool getfile;

extern QClipboard *clipboard;

#endif // ADBLOGDIALOG_H
