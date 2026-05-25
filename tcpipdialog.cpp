#include "tcpipdialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

int tcpon = 0;

tcpipDialog::tcpipDialog(QWidget *parent) :
    QDialog(parent)
{
    resize(417, 318);
    setWindowTitle(QStringLiteral("ADB/TCPIP"));

    auto *label = new QLabel(this);
    label->setGeometry(80, 30, 281, 61);
    label->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);
    label->setWordWrap(true);
    label->setText(QStringLiteral("This dialog will temporarily enable ADB debugging over WIFI.  This is useful only if your device does not enable this function by default."));

    auto *label_2 = new QLabel(this);
    label_2->setGeometry(80, 100, 281, 61);
    label_2->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);
    label_2->setWordWrap(true);
    label_2->setText(QStringLiteral("ADB over WIFI will remain enabled until you reboot your device. If you reboot,  you must reconnect via USB and re-enable."));

    m_tcplabel = new QLabel(QStringLiteral("TextLabel"), this);
    m_tcplabel->setObjectName(QStringLiteral("tcplabel"));
    m_tcplabel->setGeometry(110, 170, 221, 16);

    auto *buttonWidget = new QWidget(this);
    buttonWidget->setGeometry(100, 230, 205, 32);
    auto *horizontalLayout = new QHBoxLayout(buttonWidget);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    m_tcpipCancel = new QPushButton(QStringLiteral("Cancel"), buttonWidget);
    m_tcpipCancel->setObjectName(QStringLiteral("tcpipCancel"));
    horizontalLayout->addWidget(m_tcpipCancel);

    m_tcpipOn = new QPushButton(QStringLiteral("Enable"), buttonWidget);
    m_tcpipOn->setObjectName(QStringLiteral("tcpipOn"));
    horizontalLayout->addWidget(m_tcpipOn);

    connect(m_tcpipCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_tcpipOn, &QPushButton::clicked, this, &QDialog::accept);
}

tcpipDialog::~tcpipDialog()
{
}

void tcpipDialog::settcplabel(const QString &labeltext)
{
   m_tcplabel->setText(labeltext);
}

void tcpipDialog::on_tcpipOn_clicked()
{
    tcpon = 1;
}

int tcpipDialog::returntcp() {
   return tcpon;
}
