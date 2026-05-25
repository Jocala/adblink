#include "searchdialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

searchDialog::searchDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(275, 206);
    setWindowTitle(QStringLiteral("Find/Replace"));

    auto *label = new QLabel(QStringLiteral("Find:"), this);
    label->setGeometry(60, 20, 81, 16);

    m_fstring = new QLineEdit(this);
    m_fstring->setObjectName(QStringLiteral("fstring"));
    m_fstring->setGeometry(60, 40, 151, 21);

    auto *label_2 = new QLabel(QStringLiteral("Replace with:"), this);
    label_2->setGeometry(60, 70, 131, 16);

    m_rstring = new QLineEdit(this);
    m_rstring->setObjectName(QStringLiteral("rstring"));
    m_rstring->setGeometry(60, 90, 151, 21);

    auto *buttonWidget = new QWidget(this);
    buttonWidget->setGeometry(50, 130, 164, 32);
    auto *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    m_scancelButton = new QPushButton(QStringLiteral("Cancel"), buttonWidget);
    m_scancelButton->setObjectName(QStringLiteral("scancelButton"));
    buttonLayout->addWidget(m_scancelButton);

    m_pushButton_2 = new QPushButton(QStringLiteral("OK"), buttonWidget);
    m_pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
    buttonLayout->addWidget(m_pushButton_2);

    connect(m_scancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_pushButton_2, &QPushButton::clicked, this, &QDialog::accept);
}

searchDialog::~searchDialog()
{
}

void searchDialog::on_pushButton_2_clicked()
{
}

QString searchDialog::esearch1() {
   return m_fstring->text();
}

QString searchDialog::esearch2() {
   return m_rstring->text();
}
