#ifndef RESTDIALOG_H
#define RESTDIALOG_H

#include <QDialog>
#include <QStringList>

class QLabel;
class QListWidget;
class QPushButton;

class restDialog : public QDialog
{
    Q_OBJECT

public:
    explicit restDialog(QWidget *parent = nullptr);
    ~restDialog();

    void setadb_restore(const QStringList &list);
    QString restore_data_root();

private slots:
    void on_listDirectories_doubleClicked();
    void on_okButton_clicked();

private:
    QListWidget *m_listDirectories;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
};

#endif // RESTDIALOG_H
