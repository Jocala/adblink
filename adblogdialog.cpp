#include "adblogdialog.h"

#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QClipboard>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QApplication>

#ifdef Q_OS_WIN
QString logdir = QDir::homePath() + "/AppData/Roaming/.jocala/";
#elif defined(Q_OS_UNIX)
QString logdir = QDir::homePath() + "/.jocala/";
#endif

QString logfile1 = "adblink.log";
QString logfile2 = "adblink.old.log";
QString content;
bool getfile=true;

QClipboard *clipboard = nullptr;

void getlog()
{
    QString fn = getfile ? logfile1 : logfile2;
    QFile file(logdir + fn);

    if (!file.exists()) {
        QMessageBox::critical(0, "", "Can't find " + fn + "\n", QMessageBox::Cancel);
        getfile = true;
        return;
    }

    (void)file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    content = stream.readAll();
    file.close();
}

adblogDialog::adblogDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Logfile Viewer");
    resize(598, 602);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *mainLayout = new QVBoxLayout(this);

    logfileName = new QLabel("filename", this);
    mainLayout->addWidget(logfileName);

    logBrowser = new QTextBrowser(this);
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

    connect(quitButton, &QPushButton::clicked, this, &QDialog::close);

    getlog();
    logfileName->setText("adblink.log");
    logBrowser->setPlainText(content);
}

adblogDialog::~adblogDialog()
{
}

void adblogDialog::on_copyButton_clicked()
{
    clipboard->setText(content);
}

void adblogDialog::on_swapButton_clicked()
{
    getfile = !getfile;

    logfileName->setText(getfile ? "adblink.log" : "adblink.old.log");

    getlog();
    logBrowser->setPlainText(content);
}
