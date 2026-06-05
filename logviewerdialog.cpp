#include "logviewerdialog.h"
#include "getadbdata.h"

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QTextStream>
#include <QVBoxLayout>

LogViewerDialog::LogViewerDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Log viewer"));
    setMinimumSize(615, 595);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *mainLayout = new QVBoxLayout(this);

    fileNameLabel = new QLabel(QStringLiteral("filename"), this);
    mainLayout->addWidget(fileNameLabel);

    logBrowser = new QTextBrowser(this);
    logBrowser->setMinimumSize(570, 480);
    mainLayout->addWidget(logBrowser);

    auto *buttonLayout = new QHBoxLayout();

    copyButton = new QPushButton(QStringLiteral("Copy"), this);
    copyButton->setToolTip(QStringLiteral("Copy to clipboard"));
    buttonLayout->addWidget(copyButton);

    swapButton = new QPushButton(QStringLiteral("Swap"), this);
    swapButton->setToolTip(QStringLiteral("Switch between current and former logs."));
    buttonLayout->addWidget(swapButton);

    quitButton = new QPushButton(QStringLiteral("Quit"), this);
    quitButton->setToolTip(QStringLiteral("Quit viewer"));
    buttonLayout->addWidget(quitButton);

    mainLayout->addLayout(buttonLayout);

    connect(copyButton, &QPushButton::clicked, this, &LogViewerDialog::copyClicked);
    connect(swapButton, &QPushButton::clicked, this, &LogViewerDialog::swapClicked);
    connect(quitButton, &QPushButton::clicked, this, &QDialog::close);
}

LogViewerDialog::~LogViewerDialog() = default;

void LogViewerDialog::loadAdblinkLog()
{
    m_isRemote = false;
    m_logDir = QDir::homePath() + QStringLiteral("/.jocala/");
#ifdef Q_OS_WIN
    m_logDir = QDir::homePath() + QStringLiteral("/AppData/Roaming/.jocala/");
#endif

    QString path = m_logDir + (m_showingFirst ? m_firstLogName : m_secondLogName);
    QFile file(path);
    if (!file.exists()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Can't find %1\n").arg(m_showingFirst ? m_firstLogName : m_secondLogName));
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        m_showingFirst = true;
        return;
    }

    (void)file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();

    showContent(content, m_showingFirst ? m_firstLogName : m_secondLogName);
}

void LogViewerDialog::loadRemoteLog(const QString &adbPath, const QString &filepath)
{
    m_isRemote = true;
    m_adbPath = adbPath;
    m_remoteFilePath = filepath;

    QString filename = m_showingFirst ? QStringLiteral("kodi.log") : QStringLiteral("kodi.old.log");
    QString cstring = adbPath + QStringLiteral(" shell cat ") + filepath + filename;
    QString content = getadbOutput(cstring);

    showContent(content, filename);
}

void LogViewerDialog::loadLocalLog(const QString &ostype)
{
    m_isRemote = false;
    int type = ostype.toInt();

    switch (type) {
    case 1:
        m_firstContent = readFile(QDir::homePath() + QStringLiteral("/AppData/Roaming/Kodi/kodi.log"),
                                  QStringLiteral("Log file not found at ") + QDir::homePath() + QStringLiteral("/AppData/Roaming/Kodi/kodi.log"));
        m_secondContent = readFile(QDir::homePath() + QStringLiteral("/AppData/Roaming/Kodi/kodi.old.log"),
                                   QStringLiteral("Log file not found at ") + QDir::homePath() + QStringLiteral("/AppData/Roaming/Kodi/kodi.old.log"));
        break;
    case 2:
        m_firstContent = readFile(QDir::homePath() + QStringLiteral("/Library/Logs/kodi.log"),
                                  QStringLiteral("Log file not found at ") + QDir::homePath() + QStringLiteral("/Library/Logs/kodi.log"));
        m_secondContent = readFile(QDir::homePath() + QStringLiteral("/Library/Logs/kodi.old.log"),
                                   QStringLiteral("Log file not found at ") + QDir::homePath() + QStringLiteral("/Library/Logs/kodi.old.log"));
        break;
    case 3:
        m_firstContent = readFile(QDir::homePath() + QStringLiteral("/.kodi/kodi.log"),
                                  QStringLiteral("Log file not found at ") + QDir::homePath() + QStringLiteral("/.kodi/kodi.log"));
        m_secondContent = readFile(QDir::homePath() + QStringLiteral("/.kodi/kodi.old.log"),
                                   QStringLiteral("Log file not found at ") + QDir::homePath() + QStringLiteral("/.kodi/kodi.old.log"));
        break;
    default:
        return;
    }

    showContent(m_firstContent, QStringLiteral("kodi.log"));
}

void LogViewerDialog::copyClicked()
{
    QApplication::clipboard()->setText(m_currentContent);
}

void LogViewerDialog::swapClicked()
{
    m_showingFirst = !m_showingFirst;

    if (m_isRemote && !m_adbPath.isEmpty()) {
        QString filename = m_showingFirst ? QStringLiteral("kodi.log") : QStringLiteral("kodi.old.log");
        QString cstring = m_adbPath + QStringLiteral(" shell cat ") + m_remoteFilePath + filename;
        QString content = getadbOutput(cstring);
        if (content.contains(QStringLiteral("No such file or directory")))
            content.clear();
        showContent(content, filename);
    } else {
        QString filename = m_showingFirst ? m_firstLogName : m_secondLogName;
        if (m_firstContent.isEmpty() && m_secondContent.isEmpty()) {
            loadAdblinkLog();
        } else {
            showContent(m_showingFirst ? m_firstContent : m_secondContent, filename);
        }
    }
}

void LogViewerDialog::showContent(const QString &content, const QString &filename)
{
    fileNameLabel->setText(filename);
    logBrowser->setPlainText(content);
    m_currentContent = content;
}

QString LogViewerDialog::readFile(const QString &path, const QString &errorMsg)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&file);
        return in.readAll();
    }
    return errorMsg;
}
