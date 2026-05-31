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
    void getlog();

    QString m_logdir;
    QString m_logfile1;
    QString m_logfile2;
    QString m_content;
    bool m_getfile;
    QClipboard *m_clipboard;

    QLabel *logfileName;
    QTextBrowser *logBrowser;
    QPushButton *copyButton;
    QPushButton *swapButton;
    QPushButton *quitButton;
};

#endif // ADBLOGDIALOG_H
