#include "scpdialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

scpDialog::scpDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("ScrCpy");
    setFixedSize(370, 146);

    auto *label = new QLabel("ScrCpy arguments (optional)");

    scpargs = new QLineEdit;
    scpargs->setToolTip("Enter any arguments for ScrCpy. To see valid arguments, enter  \"scrcpy --help\"  from the command-line.");

    auto *cancelButton = new QPushButton("Cancel");
    cancelButton->setToolTip("Cancel Scrcpy");
    cancelButton->setAutoDefault(false);

    auto *clearButton = new QPushButton("Clear");
    clearButton->setToolTip("Clear arguments field");
    clearButton->setAutoDefault(false);

    auto *okButton = new QPushButton("OK");
    okButton->setToolTip("Start ScrCpy");

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(okButton);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addWidget(scpargs);
    mainLayout->addLayout(buttonLayout);

    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(clearButton, &QPushButton::clicked, this, &scpDialog::on_pushButton_clicked);
}

scpDialog::~scpDialog()
{
}

QString scpDialog::scpArgs()
{
    return scpargs->text();
}

void scpDialog::setArgs(QString myargs)
{
    scpargs->setText(myargs);
}

void scpDialog::on_pushButton_clicked()
{
    scpargs->setText("");
}
