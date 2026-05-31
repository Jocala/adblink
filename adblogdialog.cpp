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

void adblogDialog::getlog()
{
    QString fn = m_getfile ? m_logfile1 : m_logfile2;
    QFile file(m_logdir + fn);

    if (!file.exists()) {
        QMessageBox::critical(0, "", "Can't find " + fn + "\n", QMessageBox::Cancel);
        m_getfile = true;
        return;
    }

    (void)file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    m_content = stream.readAll();
    file.close();
}

adblogDialog::adblogDialog(QWidget *parent) :
    QDialog(parent),
    m_logdir(QDir::homePath() + "/.jocala/"),
    m_logfile1("adblink.log"),
    m_logfile2("adblink.old.log"),
    m_getfile(true),
    m_clipboard(nullptr)
{
#ifdef Q_OS_WIN
    m_logdir = QDir::homePath() + "/AppData/Roaming/.jocala/";
#endif

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
    logBrowser->setPlainText(m_content);
}

adblogDialog::~adblogDialog()
{
}

void adblogDialog::on_copyButton_clicked()
{
    m_clipboard->setText(m_content);
}

void adblogDialog::on_swapButton_clicked()
{
    m_getfile = !m_getfile;

    logfileName->setText(m_getfile ? "adblink.log" : "adblink.old.log");

    getlog();
    logBrowser->setPlainText(m_content);
}
