#include "sleepdialog.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

sleepDialog::sleepDialog(QWidget *parent) :
    QDialog(parent)
{
    setFixedSize(450, 300);
    setWindowTitle(QStringLiteral("Set timers"));

    m_title = new QLabel(QStringLiteral("Sleep & Screensaver"), this);
    m_title->setObjectName(QStringLiteral("title"));
    m_title->setGeometry(100, 10, 211, 16);
    m_title->adjustSize();
    m_title->move((450 - m_title->size().width()) / 2, 10);

    m_deviceLabel = new QLabel(QStringLiteral("devicename"), this);
    m_deviceLabel->setObjectName(QStringLiteral("deviceLabel"));
    m_deviceLabel->setGeometry(150, 36, 81, 16);
    m_deviceLabel->adjustSize();
    m_deviceLabel->move((450 - m_deviceLabel->size().width()) / 2, 30);

    auto *sleepGroup = new QGroupBox(QStringLiteral("Sleep"), this);
    sleepGroup->setGeometry(30, 60, 160, 130);

    auto *sleepLayoutWidget = new QWidget(sleepGroup);
    sleepLayoutWidget->setGeometry(10, 30, 128, 99);
    auto *sleepLayout = new QVBoxLayout(sleepLayoutWidget);
    sleepLayout->setContentsMargins(0, 0, 0, 0);

    m_sleepOff = new QRadioButton(QStringLiteral("Sleep off"), sleepLayoutWidget);
    m_sleepOff->setObjectName(QStringLiteral("sleepOff"));
    sleepLayout->addWidget(m_sleepOff);

    m_sleepOn = new QRadioButton(QStringLiteral("Sleep on"), sleepLayoutWidget);
    m_sleepOn->setObjectName(QStringLiteral("sleepOn"));
    sleepLayout->addWidget(m_sleepOn);

    auto *sleepValueLabel = new QLabel(QStringLiteral("Current value:"), sleepLayoutWidget);
    sleepValueLabel->setMinimumHeight(21);
    sleepValueLabel->setMaximumHeight(21);
    sleepLayout->addWidget(sleepValueLabel);

    m_sleepEdit = new QLineEdit(sleepLayoutWidget);
    m_sleepEdit->setObjectName(QStringLiteral("sleepEdit"));
    m_sleepEdit->setToolTip(QStringLiteral("Sleep timer start value in milliseconds"));
    m_sleepEdit->setInputMask(QStringLiteral("#9999999999"));
    sleepLayout->addWidget(m_sleepEdit);

    auto *screenGroup = new QGroupBox(QStringLiteral("Screensaver"), this);
    screenGroup->setGeometry(250, 60, 160, 130);

    auto *screenLayoutWidget = new QWidget(screenGroup);
    screenLayoutWidget->setGeometry(10, 30, 128, 99);
    auto *screenLayout = new QVBoxLayout(screenLayoutWidget);
    screenLayout->setContentsMargins(0, 0, 0, 0);

    m_screenOff = new QRadioButton(QStringLiteral("Screensaver off"), screenLayoutWidget);
    m_screenOff->setObjectName(QStringLiteral("screenOff"));
    screenLayout->addWidget(m_screenOff);

    m_screenOn = new QRadioButton(QStringLiteral("Screensaver on"), screenLayoutWidget);
    m_screenOn->setObjectName(QStringLiteral("screenOn"));
    screenLayout->addWidget(m_screenOn);

    auto *screenValueLabel = new QLabel(QStringLiteral("Current value:"), screenLayoutWidget);
    screenValueLabel->setMinimumHeight(21);
    screenValueLabel->setMaximumHeight(21);
    screenLayout->addWidget(screenValueLabel);

    m_screenEdit = new QLineEdit(screenLayoutWidget);
    m_screenEdit->setObjectName(QStringLiteral("screenEdit"));
    m_screenEdit->setToolTip(QStringLiteral("Screensaver start time in milliseconds"));
    m_screenEdit->setInputMask(QStringLiteral("#9999999999"));
    screenLayout->addWidget(m_screenEdit);

    m_okButton = new QPushButton(QStringLiteral("OK"), this);
    m_okButton->setObjectName(QStringLiteral("okButton"));
    m_okButton->setGeometry(240, 220, 114, 32);

    m_cancelButton = new QPushButton(QStringLiteral("Cancel"), this);
    m_cancelButton->setObjectName(QStringLiteral("cancelButton"));
    m_cancelButton->setGeometry(70, 220, 114, 32);

    m_androidLabel = new QLabel(QStringLiteral("android"), this);
    m_androidLabel->setObjectName(QStringLiteral("androidLabel"));
    m_androidLabel->setGeometry(180, 490, 46, 16);

    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

sleepDialog::~sleepDialog()
{
}

QString sleepDialog::sleepValue() {
    return m_sleepEdit->text();
}

QString sleepDialog::screenValue() {
    return m_screenEdit->text();
}

void sleepDialog::setcurrentsleep(const QString &csleep)
{
    m_sleepEdit->setText(csleep);
}

void sleepDialog::setcurrentscreen(const QString &cscreen)
{
    m_screenEdit->setText(cscreen);

    if (cscreen == "30000")
    {
        m_screenOff->setChecked(false);
        m_screenOn->setChecked(true);
    }
    else
    {
        m_screenOff->setChecked(true);
        m_screenOn->setChecked(false);
    }
}

void sleepDialog::setdevicelabel(const QString &description)
{
    m_deviceLabel->setText(description);
}

void sleepDialog::setandroidlabel(const QString &android)
{
    m_androidLabel->setText(android);
    m_andos = m_androidLabel->text().toInt();

    if (m_andos >= 11)
    {
        if (m_sleepEdit->text() == "1")
        {
            m_sleepOff->setChecked(true);
            m_sleepOn->setChecked(false);
        }
        else
        {
            m_sleepOff->setChecked(false);
            m_sleepOn->setChecked(true);
        }
    }
    else
    {
        if (m_sleepEdit->text() == "0")
        {
            m_sleepOff->setChecked(true);
            m_sleepOn->setChecked(false);
        }
        else
        {
            m_sleepOff->setChecked(false);
            m_sleepOn->setChecked(true);
        }
    }
}

void sleepDialog::on_sleepOff_clicked()
{
    if (m_androidLabel->text().toInt() >= 11)
        m_sleepEdit->setText(QStringLiteral("1"));
    else
        m_sleepEdit->setText(QStringLiteral("0"));
}

void sleepDialog::on_sleepOn_clicked()
{
    if (m_androidLabel->text().toInt() >= 11)
        m_sleepEdit->setText(QStringLiteral("0"));
    else
        m_sleepEdit->setText(QStringLiteral("1200000"));
}

void sleepDialog::on_screenOff_clicked()
{
    m_screenEdit->setText(QStringLiteral("2147460000"));
    m_screenOff->setChecked(true);
    m_screenOn->setChecked(false);
}

void sleepDialog::on_screenOn_clicked()
{
    m_screenEdit->setText(QStringLiteral("300000"));
    m_screenOff->setChecked(false);
    m_screenOn->setChecked(true);
}
