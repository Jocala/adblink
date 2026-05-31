#ifndef DATADIALOG_H
#define DATADIALOG_H

#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include <QRadioButton>
#include <QPushButton>

class dataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit dataDialog(QWidget *parent = nullptr);
    ~dataDialog();

    int returnval2();
    QString externalLocation();
    void setadb_data(const QStringList &list);

private slots:
    void on_dataButton1_clicked();
    void on_dataButton2_clicked();
//    void on_dataButton3_clicked();
//    void on_dataButton4_clicked();
    void on_okButton_clicked();
    void on_listDirectories1_doubleClicked();

private:
    int m_rval2;
    QString m_externalLocation;

    QLabel *m_labelSelectPartition;
    QListWidget *m_listDirectories;
    QLabel *m_labelChooseOperation;
    QRadioButton *m_dataButton1;
    QRadioButton *m_dataButton2;
//    QRadioButton *m_dataButton3;
//    QRadioButton *m_dataButton4;
    QPushButton *m_cancelButton;
    QPushButton *m_okButton;
};

#endif // DATADIALOG_H
