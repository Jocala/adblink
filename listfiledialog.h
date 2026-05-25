#ifndef LISTFILEDIALOG_H
#define LISTFILEDIALOG_H

#include <QDialog>
#include <QStringList>

class QListWidget;
class QPushButton;

class listfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit listfileDialog(QWidget *parent = 0);
    ~listfileDialog();

    void setFilelist(const QStringList &filelist);
    void setDialogTitle(const QString &dtitle);
    QString return_fitem();

    QString fitem;

private slots:
    void on_selectButton_clicked();
    void on_flistWidget_doubleClicked(const QModelIndex &index);

private:
    QListWidget *m_flistWidget;
    QPushButton *m_selectButton;
    QPushButton *m_cancelButton;
};

#endif // LISTFILEDIALOG_H
