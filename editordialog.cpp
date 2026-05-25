#include "editordialog.h"
#include "searchdialog.h"

#include <QClipboard>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

#ifdef Q_OS_LINUX
 int edos=0;
#elif defined(Q_OS_WIN)
  int edos=1;
#elif defined(Q_OS_MAC)
int edos=2;
#endif

QClipboard *edclipboard = nullptr;

editorDialog::editorDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(580, 610);
    setMinimumSize(300, 200);
    setWindowTitle(QStringLiteral("editor"));

    auto *mainLayout = new QVBoxLayout(this);

    m_editorfilename = new QLabel(QStringLiteral("filename"), this);
    m_editorfilename->setObjectName(QStringLiteral("editorfilename"));
    mainLayout->addWidget(m_editorfilename);

    m_editorBrowser = new QTextBrowser(this);
    m_editorBrowser->setObjectName(QStringLiteral("editorBrowser"));
    m_editorBrowser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_editorBrowser->setReadOnly(false);
    mainLayout->addWidget(m_editorBrowser, 1);

    auto *buttonLayout = new QHBoxLayout;

    m_editquitButton = new QPushButton(QStringLiteral("Cancel"), this);
    m_editquitButton->setObjectName(QStringLiteral("editquitButton"));
    buttonLayout->addWidget(m_editquitButton);

    m_copyButton = new QPushButton(QStringLiteral("Copy"), this);
    m_copyButton->setObjectName(QStringLiteral("copyButton"));
    buttonLayout->addWidget(m_copyButton);

    m_searchButton = new QPushButton(QStringLiteral("Search"), this);
    m_searchButton->setObjectName(QStringLiteral("searchButton"));
    buttonLayout->addWidget(m_searchButton);

    m_saveButton = new QPushButton(QStringLiteral("Save"), this);
    m_saveButton->setObjectName(QStringLiteral("saveButton"));
    buttonLayout->addWidget(m_saveButton);

    mainLayout->addLayout(buttonLayout);

    connect(m_editquitButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveButton, &QPushButton::clicked, this, &QDialog::accept);
}

editorDialog::~editorDialog()
{
}

void editorDialog::seteditor(const QString &xmlfile)
{
    m_editorBrowser->setText(xmlfile);
}

void editorDialog::on_copyButton_clicked()
{
    edclipboard->setText(m_editorBrowser->toPlainText());
}

QString editorDialog::xmlfile() {
    return m_editorBrowser->toPlainText();
}

void editorDialog::setfilename(const QString &filename)
{
    m_editorfilename->setText(filename);
}

void editorDialog::on_searchButton_clicked()
{
    searchDialog sdialog;

    sdialog.setModal(true);

     if(sdialog.exec() == QDialog::Accepted)
       {
        QString fstring = sdialog.esearch1();
        QString rstring = sdialog.esearch2();
        QString tempstring = m_editorBrowser->toPlainText();
        tempstring.replace(fstring,rstring);
        m_editorBrowser->setText(tempstring);
       }
}
