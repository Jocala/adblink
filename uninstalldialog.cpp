#include "uninstalldialog.h"
#include <QString>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "adbutils.h"
#include "getadbdata.h"
#include "logfile.h"

QString uninstallDialog::packageName() {
   if (m_unlistWidget->selectedItems().count() == 1 )
        return m_unlistWidget->currentItem()->text();
     else return "";
}

bool uninstallDialog::keepBox() {
   return m_keepBox->isChecked();
}

uninstallDialog::uninstallDialog(const QString &adbPrefix, QWidget *parent) :
   QDialog(parent), m_adbPrefix(adbPrefix.trimmed())
{
   QString title = m_adbPrefix;
   int sIdx = title.indexOf(" -s ");
   if (sIdx >= 0)
       title = title.mid(sIdx + 4).trimmed();
   setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
   setWindowTitle(title);

   setFixedSize(425, 370);
   setSizeGripEnabled(true);

   m_titleLabel = new QLabel("Uninstall APK", this);

   m_lineEdit = new QLineEdit(this);

   m_applyButton = new QPushButton("Apply", this);
   m_applyButton->setToolTip("Apply filter to package list");

   m_clearButton = new QPushButton("Clear", this);
   m_clearButton->setToolTip("Clear filter and reset package list");

   m_keepBox = new QCheckBox("-k (keep app data)", this);
   m_keepBox->setToolTip("keep application data");

   m_unlistWidget = new QListWidget(this);
   m_unlistWidget->setMinimumSize(370, 190);
   m_unlistWidget->setMaximumSize(370, 190);
   m_unlistWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

   m_cancelButton = new QPushButton("Cancel", this);
   m_cancelButton->setMaximumWidth(150);

   m_okButton = new QPushButton("OK", this);
   m_okButton->setMaximumWidth(150);
   m_okButton->setToolTip("Uninstall APK");

   QVBoxLayout *mainLayout = new QVBoxLayout(this);
   mainLayout->setContentsMargins(10, 20, 10, 20);
   mainLayout->setSpacing(6);

   mainLayout->addWidget(m_titleLabel);

   QHBoxLayout *filterLayout = new QHBoxLayout();
   filterLayout->addWidget(new QLabel("Filter", this));
   filterLayout->addWidget(m_lineEdit);
   filterLayout->addWidget(m_applyButton);
   filterLayout->addWidget(m_clearButton);
   mainLayout->addLayout(filterLayout);

   mainLayout->addWidget(m_keepBox);

   QVBoxLayout *listLayout = new QVBoxLayout();
   listLayout->addWidget(m_unlistWidget);
   mainLayout->addLayout(listLayout);

   QHBoxLayout *buttonLayout = new QHBoxLayout();
   buttonLayout->addWidget(m_cancelButton);
   buttonLayout->addWidget(m_okButton);
   mainLayout->addLayout(buttonLayout);

   connect(m_applyButton, &QPushButton::clicked, this, &uninstallDialog::on_applyButton_clicked);
   connect(m_clearButton, &QPushButton::clicked, this, &uninstallDialog::on_apkclearButton_clicked);
   connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
   connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
   connect(m_unlistWidget, &QListWidget::itemDoubleClicked, this, &QDialog::accept);

   loadList();
   loadBox();
}

uninstallDialog::uninstallDialog(const QString &daddr, const QString &port, QWidget *parent) :
   QDialog(parent)
{
   QString d = daddr.trimmed();
   QString p = port.trimmed();
   if (p.isEmpty())
       m_adbPrefix = QString("\"%1\" -s %2").arg(getadbpath(), d);
   else
       m_adbPrefix = QString("\"%1\" -s %2:%3").arg(getadbpath(), d, p);
   QString title = d + (p.isEmpty() ? QString() : ":" + p);
   setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
   setWindowTitle(title);
   setFixedSize(425, 370);
   setSizeGripEnabled(true);

   m_titleLabel = new QLabel("Uninstall APK", this);
   m_lineEdit = new QLineEdit(this);
   m_applyButton = new QPushButton("Apply", this);
   m_applyButton->setToolTip("Apply filter to package list");
   m_clearButton = new QPushButton("Clear", this);
   m_clearButton->setToolTip("Clear filter and reset package list");
   m_keepBox = new QCheckBox("-k (keep app data)", this);
   m_keepBox->setToolTip("keep application data");
   m_unlistWidget = new QListWidget(this);
   m_unlistWidget->setMinimumSize(370, 190);
   m_unlistWidget->setMaximumSize(370, 190);
   m_unlistWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
   m_cancelButton = new QPushButton("Cancel", this);
   m_cancelButton->setMaximumWidth(150);
   m_okButton = new QPushButton("OK", this);
   m_okButton->setMaximumWidth(150);
   m_okButton->setToolTip("Uninstall APK");
   QVBoxLayout *mainLayout = new QVBoxLayout(this);
   mainLayout->setContentsMargins(10, 20, 10, 20);
   mainLayout->setSpacing(6);
   mainLayout->addWidget(m_titleLabel);
   QHBoxLayout *filterLayout = new QHBoxLayout();
   filterLayout->addWidget(new QLabel("Filter", this));
   filterLayout->addWidget(m_lineEdit);
   filterLayout->addWidget(m_applyButton);
   filterLayout->addWidget(m_clearButton);
   mainLayout->addLayout(filterLayout);
   mainLayout->addWidget(m_keepBox);
   QVBoxLayout *listLayout = new QVBoxLayout();
   listLayout->addWidget(m_unlistWidget);
   mainLayout->addLayout(listLayout);
   QHBoxLayout *buttonLayout = new QHBoxLayout();
   buttonLayout->addWidget(m_cancelButton);
   buttonLayout->addWidget(m_okButton);
   mainLayout->addLayout(buttonLayout);
   connect(m_applyButton, &QPushButton::clicked, this, &uninstallDialog::on_applyButton_clicked);
   connect(m_clearButton, &QPushButton::clicked, this, &uninstallDialog::on_apkclearButton_clicked);
   connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
   connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
   connect(m_unlistWidget, &QListWidget::itemDoubleClicked, this, &QDialog::accept);
   loadList();
   loadBox();
}

uninstallDialog::~uninstallDialog()
{
}

void uninstallDialog::on_applyButton_clicked() {
    QString filter = m_lineEdit->text().trimmed();
    if (filter.isEmpty()) {
        loadList();
        loadBox();
        return;
    }
    if (m_lastOutput.isEmpty())
        loadList();
    QStringList packageList = m_lastOutput.split('\n', Qt::SkipEmptyParts);
    QStringList filtered;
    for (const QString &line : packageList) {
        if (!line.startsWith("package:"))
            continue;
        QString pkg = line.mid(8).trimmed();
        if (pkg.contains(filter, Qt::CaseInsensitive))
            filtered.append(line);
    }
    m_unlistWidget->clear();
    for (const QString &line : filtered) {
        QString pkg = line.mid(8).trimmed();
        if (!pkg.isEmpty())
            m_unlistWidget->addItem(pkg);
    }
    logfile(QString("uninstall filter \"%1\" matched %2/%3").arg(filter).arg(filtered.size()).arg(packageList.size()));
}

void uninstallDialog::loadList() {
    QString cmd = m_adbPrefix + " shell pm list packages";
    logfile("uninstall loadList: " + cmd);
    m_lastOutput = getadbOutput(cmd);
    logfile(QString("uninstall pm list bytes=%1").arg(m_lastOutput.size()));
    if (!m_lastOutput.isEmpty())
        logfile("uninstall pm list head: " + m_lastOutput.left(400).replace("\n", " | "));
    if (m_lastOutput.isEmpty() || m_lastOutput.contains("error:", Qt::CaseInsensitive)
        || m_lastOutput.contains("not found", Qt::CaseInsensitive)
        || m_lastOutput.contains("unauthorized", Qt::CaseInsensitive)
        || m_lastOutput.contains("offline", Qt::CaseInsensitive)
        || m_lastOutput.contains("no devices", Qt::CaseInsensitive)) {
        logfile("uninstall pm list warning: " + m_lastOutput.trimmed().left(500));
    }
}

void uninstallDialog::loadBox() {
    m_unlistWidget->clear();
    QStringList packageList = m_lastOutput.split('\n', Qt::SkipEmptyParts);
    int added = 0;
    for (QString package : packageList) {
        package = package.trimmed();
        if (package.isEmpty())
            continue;
        if (!package.startsWith("package:")) {
            if (package.contains("error:", Qt::CaseInsensitive)
                || package.contains("offline", Qt::CaseInsensitive)
                || package.contains("unauthorized", Qt::CaseInsensitive))
                logfile("uninstall loadBox skip: " + package);
            continue;
        }
        package = package.mid(8).trimmed();
        if (!package.isEmpty()) {
            m_unlistWidget->addItem(package);
            ++added;
        }
    }
    logfile(QString("uninstall loadBox added %1 items").arg(added));
    if (added == 0 && !m_lastOutput.trimmed().isEmpty()) {
        logfile("uninstall loadBox empty after parse, raw: " + m_lastOutput.left(500).replace("\n", " | "));
    }
}

void uninstallDialog::on_apkclearButton_clicked() {
    m_lineEdit->setText("");
    loadList();
    loadBox();
}
