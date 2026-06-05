#ifndef LOGVIEWERDIALOG_H
#define LOGVIEWERDIALOG_H

#include <QDialog>

class QLabel;
class QTextBrowser;
class QPushButton;

class LogViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogViewerDialog(QWidget *parent = nullptr);
    ~LogViewerDialog();

    void loadAdblinkLog();
    void loadRemoteLog(const QString &adbPath, const QString &filepath);
    void loadLocalLog(const QString &ostype);

private slots:
    void copyClicked();
    void swapClicked();

private:
    void showContent(const QString &content, const QString &filename);

    QLabel *fileNameLabel;
    QTextBrowser *logBrowser;
    QPushButton *copyButton;
    QPushButton *swapButton;
    QPushButton *quitButton;

    QString m_currentContent;
    bool m_showingFirst = true;

    // Adblink log state
    QString m_logDir;
    QString m_firstLogName = QStringLiteral("adblink.log");
    QString m_secondLogName = QStringLiteral("adblink.old.log");

    // Remote log state
    QString m_adbPath;
    QString m_remoteFilePath;
    bool m_isRemote = false;

    // Local Kodi log content cache
    QString m_firstContent;
    QString m_secondContent;

    static QString readFile(const QString &path, const QString &errorMsg);
};

#endif // LOGVIEWERDIALOG_H
