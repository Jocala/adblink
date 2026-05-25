#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;

class searchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit searchDialog(QWidget *parent = 0);
    ~searchDialog();

    QString esearch1();
    QString esearch2();

private slots:
    void on_pushButton_2_clicked();

private:
    QLineEdit *m_fstring;
    QLineEdit *m_rstring;
    QPushButton *m_scancelButton;
    QPushButton *m_pushButton_2;
};

#endif // SEARCHDIALOG_H
