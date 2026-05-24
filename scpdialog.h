#ifndef SCPDIALOG_H
#define SCPDIALOG_H

#include <QDialog>

class QLineEdit;

class scpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit scpDialog(QWidget *parent = nullptr);
    ~scpDialog();

    void setArgs(QString myargs);
    QString scpArgs();

private slots:
    void on_pushButton_clicked();

private:
    QLineEdit *scpargs;
};

#endif // SCPDIALOG_H
