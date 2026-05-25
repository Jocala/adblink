#include "helpdialog.h"

#include <QDesktopServices>
#include <QPushButton>
#include <QTextBrowser>
#include <QUrl>

helpDialog::helpDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(450, 420);
    setWindowTitle(QStringLiteral("adblink help"));

    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setObjectName(QStringLiteral("textBrowser"));
    m_textBrowser->setGeometry(50, 20, 351, 321);
    m_textBrowser->setSource(QUrl(QStringLiteral("qrc:/assets/menu.html")));
    m_textBrowser->setWindowTitle(QStringLiteral("adblink Help"));

    m_pushButton = new QPushButton(QStringLiteral("Close"), this);
    m_pushButton->setObjectName(QStringLiteral("pushButton"));
    m_pushButton->setGeometry(180, 360, 93, 28);

    connect(m_pushButton, &QPushButton::clicked, this, &QDialog::close);
}

helpDialog::~helpDialog()
{
}
