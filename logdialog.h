#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class logDialog : public QDialog
{
    Q_OBJECT

public:
    explicit logDialog(QWidget *parent = nullptr);
    ~logDialog();

    void loadRemoteLog(const QString &adbpath, const QString &filepath);
    void loadLocalLog(const QString &ostype);

private slots:
    void copyClicked();
    void swapClicked();

private:
    void setupUi();

    QLabel *fileNameLabel;
    QTextBrowser *logBrowser;
    QPushButton *copyButton;
    QPushButton *swapButton;
    QPushButton *quitButton;

    QString currentContent;
    bool showingFirst = true;
    QString firstLogName = "kodi.log";
    QString secondLogName = "kodi.old.log";

    // Remote state
    QString adbPath;
    QString remoteFilePath;

    // Local state
    QString firstLogContent;
    QString secondLogContent;

    static QString readFile(const QString &path, const QString &errorMsg);
};

#endif // LOGDIALOG_H
