#include "logdialog.h"
#include "getadbdata.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QClipboard>
#include <QApplication>

logDialog::logDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi();
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

logDialog::~logDialog()
{
}

void logDialog::setupUi()
{
    setMinimumSize(615, 595);
    setWindowTitle("Logfile viewer");

    auto *mainLayout = new QVBoxLayout(this);

    fileNameLabel = new QLabel("filename", this);
    mainLayout->addWidget(fileNameLabel);

    logBrowser = new QTextBrowser(this);
    logBrowser->setMinimumSize(570, 480);
    mainLayout->addWidget(logBrowser);

    auto *buttonLayout = new QHBoxLayout();

    copyButton = new QPushButton("Copy", this);
    copyButton->setToolTip("Copy to clipboard");
    buttonLayout->addWidget(copyButton);

    swapButton = new QPushButton("Swap", this);
    swapButton->setToolTip("Switch between current and former logs.");
    buttonLayout->addWidget(swapButton);

    quitButton = new QPushButton("Quit", this);
    quitButton->setToolTip("Quit viewer");
    buttonLayout->addWidget(quitButton);

    mainLayout->addLayout(buttonLayout);

    connect(copyButton, &QPushButton::clicked, this, &logDialog::copyClicked);
    connect(swapButton, &QPushButton::clicked, this, &logDialog::swapClicked);
    connect(quitButton, &QPushButton::clicked, this, &QDialog::reject);
}

void logDialog::loadRemoteLog(const QString &adbpath, const QString &filepath)
{
    adbPath = adbpath;
    remoteFilePath = filepath;

    fileNameLabel->setText(firstLogName);

    QString cstring = adbpath + " shell cat " + filepath + firstLogName;
    QString content = getadbOutput(cstring);

    logBrowser->setPlainText(content);
    currentContent = content;
}

void logDialog::loadLocalLog(const QString &ostype)
{
    int type = ostype.toInt();
    QString tmpdir1;

    switch (type)
    {
    case 1:
        tmpdir1 = QDir::homePath() + "/AppData/Roaming/Kodi";
        firstLogContent = readFile(tmpdir1 + "/kodi.log", "Log file not found at " + tmpdir1 + "/kodi.log");
        secondLogContent = readFile(tmpdir1 + "/kodi.old.log", "Log file not found at " + tmpdir1 + "/kodi.old.log");
        break;
    case 2:
        firstLogContent = readFile(QDir::homePath() + "/Library/Logs/kodi.log", "Log file not found at " + QDir::homePath() + "/Library/Logs/kodi.log");
        secondLogContent = readFile(QDir::homePath() + "/Library/Logs/kodi.old.log", "Log file not found at " + QDir::homePath() + "/Library/Logs/kodi.old.log");
        break;
    case 3:
        firstLogContent = readFile(QDir::homePath() + "/.kodi/kodi.log", "Log file not found at " + QDir::homePath() + "/.kodi/kodi.log");
        secondLogContent = readFile(QDir::homePath() + "/.kodi/kodi.old.log", "Log file not found at " + QDir::homePath() + "/.kodi/kodi.old.log");
        break;
    default:
        return;
    }

    fileNameLabel->setText(firstLogName);
    logBrowser->setPlainText(firstLogContent);
    currentContent = firstLogContent;
}

void logDialog::copyClicked()
{
    QApplication::clipboard()->setText(currentContent);
    accept();
}

void logDialog::swapClicked()
{
    showingFirst = !showingFirst;

    if (!adbPath.isEmpty())
    {
        QString filename = showingFirst ? firstLogName : secondLogName;
        fileNameLabel->setText(filename);
        QString cstring = adbPath + " shell cat " + remoteFilePath + filename;
        QString content = getadbOutput(cstring);
        if (content.contains("No such file or directory"))
            content = "";
        logBrowser->setPlainText(content);
        currentContent = content;
    }
    else
    {
        if (showingFirst)
        {
            fileNameLabel->setText(firstLogName);
            logBrowser->setPlainText(firstLogContent);
            currentContent = firstLogContent;
        }
        else
        {
            fileNameLabel->setText(secondLogName);
            logBrowser->setPlainText(secondLogContent);
            currentContent = secondLogContent;
        }
    }
}

QString logDialog::readFile(const QString &path, const QString &errorMsg)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&file);
        return in.readAll();
    }
    return errorMsg;
}
