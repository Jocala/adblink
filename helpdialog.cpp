#include "helpdialog.h"

#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>

helpDialog::helpDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(QStringLiteral("adblink help"));
    setMinimumSize(600, 500);
    resize(700, 550);

    auto *mainLayout = new QVBoxLayout(this);

    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setObjectName(QStringLiteral("textBrowser"));
    m_textBrowser->setSource(QUrl(QStringLiteral("qrc:/assets/menu.html")));
    mainLayout->addWidget(m_textBrowser, 1);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    m_pushButton = new QPushButton(QStringLiteral("Close"), this);
    m_pushButton->setObjectName(QStringLiteral("pushButton"));
    buttonLayout->addWidget(m_pushButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    connect(m_pushButton, &QPushButton::clicked, this, &QDialog::close);
}

helpDialog::~helpDialog() = default;
