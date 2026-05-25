#include "forcequitdialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

forcequitDialog::forcequitDialog(bool takeaction, QString appname, QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(330, 210);

    m_action = takeaction;

    m_packagename = new QLineEdit(this);
    m_packagename->setObjectName(QStringLiteral("packagename"));
    m_packagename->setGeometry(20, 80, 261, 21);
    m_packagename->setToolTip(QStringLiteral("This string is passed to Android to start or stop an Application"));
    m_packagename->setClearButtonEnabled(true);

    m_title = new QLabel(QStringLiteral("Dialog Title"), this);
    m_title->setObjectName(QStringLiteral("title"));
    m_title->setGeometry(120, 20, 141, 16);

    m_explainLabel = new QLabel(QStringLiteral("Text goes here"), this);
    m_explainLabel->setObjectName(QStringLiteral("explainLabel"));
    m_explainLabel->setGeometry(20, 50, 231, 16);

    m_spmcButton = new QRadioButton(QStringLiteral("SPMC"), this);
    m_spmcButton->setObjectName(QStringLiteral("spmcButton"));
    m_spmcButton->setGeometry(90, 480, 57, 20);
    m_spmcButton->setToolTip(QStringLiteral("<html><head/><body><p>SPMC media center</p></body></html>"));

    auto *layoutWidget = new QWidget(this);
    layoutWidget->setGeometry(30, 400, 65, 39);
    auto *verticalLayout = new QVBoxLayout(layoutWidget);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    m_kodiButton = new QRadioButton(QStringLiteral("Kodi"), layoutWidget);
    m_kodiButton->setObjectName(QStringLiteral("kodiButton"));
    m_kodiButton->setChecked(true);
    m_kodiButton->setToolTip(QStringLiteral("<html><head/><body><p>Kodi media center</p></body></html>"));
    verticalLayout->addWidget(m_kodiButton);

    m_otherButton = new QRadioButton(QStringLiteral("Other"), layoutWidget);
    m_otherButton->setObjectName(QStringLiteral("otherButton"));
    m_otherButton->setToolTip(QStringLiteral("<html><head/><body><p>Fill in file path and package name</p></body></html>"));
    verticalLayout->addWidget(m_otherButton);

    auto *buttonLayoutWidget = new QWidget(this);
    buttonLayoutWidget->setGeometry(30, 120, 241, 32);
    auto *horizontalLayout = new QHBoxLayout(buttonLayoutWidget);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    m_fqcancelButton = new QPushButton(QStringLiteral("Cancel"), buttonLayoutWidget);
    m_fqcancelButton->setObjectName(QStringLiteral("fqcancelButton"));
    m_fqcancelButton->setToolTip(QStringLiteral("Cancel"));
    horizontalLayout->addWidget(m_fqcancelButton);

    m_pushButton = new QPushButton(QStringLiteral("Default"), buttonLayoutWidget);
    m_pushButton->setObjectName(QStringLiteral("pushButton"));
    m_pushButton->setToolTip(QStringLiteral("<html><head/><body><p>Restore default value</p></body></html>"));
    horizontalLayout->addWidget(m_pushButton);

    m_fqokButton = new QPushButton(QStringLiteral("OK"), buttonLayoutWidget);
    m_fqokButton->setObjectName(QStringLiteral("fqokButton"));
    m_fqokButton->setToolTip(QStringLiteral("Execute command"));
    horizontalLayout->addWidget(m_fqokButton);

    connect(m_fqokButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_fqcancelButton, &QPushButton::clicked, this, &QDialog::reject);

    if (m_action)
    {
        m_title->setText(QStringLiteral("Start Application"));
        m_explainLabel->setText(QStringLiteral("String: app starting activity"));
        m_packagename->setText(appname);
    }
    else
    {
        m_title->setText(QStringLiteral("Stop Application"));
        m_explainLabel->setText(QStringLiteral("String: app package name"));
        m_packagename->setText(appname);
    }
}

forcequitDialog::~forcequitDialog()
{
}

QString forcequitDialog::packagename() {
   return m_packagename->text();
}

void forcequitDialog::on_kodiButton_clicked()
{
    if (m_action)
        m_packagename->setText(QStringLiteral("org.xbmc.kodi/org.xbmc.kodi.Splash"));
    else
        m_packagename->setText(QStringLiteral("org.xbmc.kodi"));
}

void forcequitDialog::on_spmcButton_clicked()
{
   if (m_action)
       m_packagename->setText(QStringLiteral("com.semperpax.spmc16/com.semperpax.spmc16.Splash"));
   else
       m_packagename->setText(QStringLiteral("com.semperpax.spmc16"));
}

void forcequitDialog::on_otherButton_clicked()
{
    m_packagename->setText(QString());
}

void forcequitDialog::on_pushButton_clicked()
{
   if (m_action)
       m_packagename->setText(QStringLiteral("org.xbmc.kodi/org.xbmc.kodi.Splash"));
   else
       m_packagename->setText(QStringLiteral("org.xbmc.kodi"));
}
