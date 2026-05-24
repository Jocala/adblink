#include "uninstalldialog.h"
#include <QProcess>
#include <QString>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "adbutils.h"

#ifdef Q_OS_LINUX
 int ost=0;
#elif defined(Q_OS_WIN)
  int ost=1;
#elif defined(Q_OS_MAC)
int ost=2;
#endif

QString commstr;
QString cstr;
QString argument;
QProcess packages;

QString uninstallDialog::packageName() {
   if (m_unlistWidget->selectedItems().count() == 1 )
        return m_unlistWidget->currentItem()->text();
     else return "";
}

bool uninstallDialog::keepBox() {
   return m_keepBox->isChecked();
}

uninstallDialog::uninstallDialog(const QString &daddr, const QString &port, QWidget *parent) :
   QDialog(parent), m_daddr(daddr), m_port(port)
{
   setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
   setWindowTitle(daddr);
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

   connect(m_applyButton, SIGNAL(clicked()), this, SLOT(on_applyButton_clicked()));
   connect(m_clearButton, SIGNAL(clicked()), this, SLOT(on_apkclearButton_clicked()));
   connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
   connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));

   loadList();
   makeFile();
   loadBox();
}

uninstallDialog::~uninstallDialog()
{
}

void uninstallDialog::on_applyButton_clicked() {
    m_unlistWidget->clear();

    if (m_lineEdit->text() != "") {
        if (m_port.isEmpty())
            argument = " -s " + m_daddr + " shell pm list packages | grep " + m_lineEdit->text();
        else
            argument = " -s " + m_daddr + ":" + m_port + " shell pm list packages | grep " + m_lineEdit->text();

         cstr = QString("\"%1\"").arg(getadbpath()) + argument;

    } else {
        loadList();
    }

    makeFile();
    loadBox();
}

void uninstallDialog::loadList() {
    if (m_port.isEmpty())
        argument = " -s " + m_daddr + " shell pm list packages";
    else
        argument = " -s " + m_daddr + ":" + m_port + " shell pm list packages";

     cstr = QString("\"%1\"").arg(getadbpath()) + argument;
}

void uninstallDialog::makeFile() {
    packages.start(cstr);
    packages.waitForFinished(-1);
    commstr = packages.readAll();
}

void uninstallDialog::loadBox() {
    QStringList packageList = commstr.split('\n', Qt::SkipEmptyParts);

    foreach (QString package, packageList) {
        if (!package.isEmpty()) {
            package.remove(0, 8);
            m_unlistWidget->addItem(package);
        }
    }
}

void uninstallDialog::on_apkclearButton_clicked() {
    m_lineEdit->setText("");
}
