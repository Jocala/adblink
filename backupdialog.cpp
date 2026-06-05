#include "backupdialog.h"
#include "getadbdata.h"

#include <QCoreApplication>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QStringList>
#include <QTextStream>

backupDialog::backupDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(265, 265);
    setWindowTitle(QStringLiteral("Kodi Backup"));

    auto *label = new QLabel(QStringLiteral("Choose partition"), this);
    label->setGeometry(70, 30, 121, 16);

    m_listDirectories_backup = new QListWidget(this);
    m_listDirectories_backup->setObjectName(QStringLiteral("listDirectories_backup"));
    m_listDirectories_backup->setGeometry(50, 60, 150, 100);
    m_listDirectories_backup->setMinimumSize(150, 100);
    m_listDirectories_backup->setMaximumSize(150, 100);
    m_listDirectories_backup->setToolTip(QStringLiteral("<html><head/><body><p>Click Kodi&quot;s root partition. Default is /sdcard/</p></body></html>"));
    m_listDirectories_backup->setStyleSheet(QStringLiteral(
        "QListWidget {"
        "    border: 2px solid #8f8f91;"
        "    border-radius: 6px;"
        "    border-color: black;"
        "}"
    ));

    m_listDirectories_backup2 = new QListWidget(this);
    m_listDirectories_backup2->setObjectName(QStringLiteral("listDirectories_backup2"));
    m_listDirectories_backup2->setGeometry(80, 440, 150, 100);
    m_listDirectories_backup2->setMinimumSize(150, 100);
    m_listDirectories_backup2->setMaximumSize(150, 100);
    m_listDirectories_backup2->setToolTip(QStringLiteral("<html><head/><body><p>Click Kodi&quot;s thumbnail partition. Default is /sdcard/</p></body></html>"));
    m_listDirectories_backup2->setStyleSheet(QStringLiteral(
        "QListWidget {"
        "    border: 2px solid #8f8f91;"
        "    border-radius: 6px;"
        "    border-color: black;"
        "}"
    ));

    m_backup_label2 = new QLabel(QStringLiteral("Thumbnails:  /sdcard/"), this);
    m_backup_label2->setObjectName(QStringLiteral("backup_label2"));
    m_backup_label2->setGeometry(80, 410, 181, 16);

    auto *buttonWidget = new QWidget(this);
    buttonWidget->setGeometry(50, 190, 164, 32);
    auto *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    m_cancelButton = new QPushButton(QStringLiteral("Cancel"), buttonWidget);
    m_cancelButton->setObjectName(QStringLiteral("cancelButton"));
    buttonLayout->addWidget(m_cancelButton);

    m_okButton = new QPushButton(QStringLiteral("OK"), buttonWidget);
    m_okButton->setObjectName(QStringLiteral("okButton"));
    buttonLayout->addWidget(m_okButton);

    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_listDirectories_backup, &QListWidget::itemDoubleClicked, this, &QDialog::accept);

    QString tmpstr;
    QString commstr;
    QString cstr;
    QString argument;
    QString fline;

#if defined(Q_OS_WIN)
    m_tmpdir_backup = QStringLiteral("./");
#elif defined(Q_OS_MAC)
    m_tmpdir_backup = QCoreApplication::applicationDirPath();
    m_tmpdir_backup = m_tmpdir_backup + QStringLiteral("/adbfiles/");
#else
    m_tmpdir_backup = QCoreApplication::applicationDirPath();
    m_tmpdir_backup = m_tmpdir_backup + QStringLiteral("/adbfiles/");
#endif
}

backupDialog::~backupDialog()
{
}

QString backupDialog::return_data_root() {
   return m_data_root_backup;
}

QString backupDialog::returnthumb() {
    return m_data_root_backup;
}

void backupDialog::setadb_backup(const QString &adbPrefix, const QString &dataRoot)
{
    QString command;
    QString cstring;
    QString mounted;

    cstring = adbPrefix + QStringLiteral(" shell /data/local/tmp/adblink/busybox find /storage/ -type d -maxdepth 2 -perm 0771");
    command = getadbOutput(cstring);

    QFile file21(m_tmpdir_backup + QStringLiteral("temp.txt"));

    if(!file21.open(QFile::WriteOnly))
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Error creating file!"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    QTextStream out1(&file21);
    out1 << command << Qt::endl;

    file21.flush();
    file21.close();

    QString tmpstr2 = m_tmpdir_backup + QStringLiteral("temp.txt");
    QString fline2;
    QFile file32(tmpstr2);

    if (!file32.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Error reading file!"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::ApplicationModal);
        msgBox.exec();
        return;
    }

    QTextStream in1(&file32);

    m_listDirectories_backup->addItem(QStringLiteral("/sdcard/"));
    m_listDirectories_backup2->addItem(QStringLiteral("/sdcard/"));

    while (!in1.atEnd())
    {
        fline2 = in1.readLine();

        if((!fline2.contains(QStringLiteral("Android")))
                && (!fline2.contains(QStringLiteral(".")))
                && (!fline2.contains(QStringLiteral("Permission denied")))
                && (!fline2.contains(QStringLiteral("emulated")))
                && (!fline2.isEmpty()))
        {
            m_listDirectories_backup->addItem(fline2 + QStringLiteral("/"));
            m_listDirectories_backup2->addItem(fline2);
        }
    }

    file32.close();
    QFile::remove(tmpstr2);

    for (int i = 0; i < m_listDirectories_backup->count(); i++)
    {
        if(m_listDirectories_backup->item(i)->text() == m_data_root_backup)
        {
            m_listDirectories_backup->setCurrentRow(i);
        }
    }
}

void backupDialog::on_listDirectories_backup_clicked()
{
    m_data_root_backup = m_listDirectories_backup->currentItem()->text();

    if(m_data_root_backup != QStringLiteral("/sdcard/"))
    {
        m_listDirectories_backup2->clearSelection();
        m_thumbnail_backup_dir = m_data_root_backup;
        m_backup_label2->setText(QStringLiteral("Thumbnails: ") + m_thumbnail_backup_dir);
    }
}

void backupDialog::on_listDirectories_backup2_clicked()
{
   m_thumbnail_backup_dir = m_listDirectories_backup2->currentItem()->text();
   m_backup_label2->setText(QStringLiteral("Thumbnails: ") + m_thumbnail_backup_dir);

    if(m_data_root_backup != QStringLiteral("/sdcard/"))
    {
        m_listDirectories_backup2->clearSelection();
        m_thumbnail_backup_dir = m_data_root_backup;
        m_backup_label2->setText(QStringLiteral("Thumbnails: ") + m_thumbnail_backup_dir);
    }
}

void backupDialog::on_okButton_clicked()
{
    m_data_root_backup = m_listDirectories_backup->currentItem()->text();
}
