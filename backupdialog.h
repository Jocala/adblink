#ifndef BACKUPDIALOG_H
#define BACKUPDIALOG_H

#include <QDialog>

class QLabel;
class QListWidget;
class QPushButton;

class backupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit backupDialog(QWidget *parent = 0);
    ~backupDialog();

    QString return_data_root();
    QString returnthumb();

    void setadb_backup(const QString &adb_backup, const QString &data_root_backup);

private slots:
    void on_listDirectories_backup_clicked();
    void on_listDirectories_backup2_clicked();
    void on_okButton_clicked();

private:
    QListWidget *m_listDirectories_backup;
    QListWidget *m_listDirectories_backup2;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QLabel *m_backup_label2;
};

extern int os_backup;
extern QString adb_backup;
extern QString adbdir_backup;
extern QString tmpdir_backup;
extern QString thumbnail_backup_dir;
extern QString data_root_backup;

#endif // BACKUPDIALOG_H
