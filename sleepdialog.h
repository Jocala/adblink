#ifndef SLEEPDIALOG_H
#define SLEEPDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;

class sleepDialog : public QDialog
{
    Q_OBJECT

public:
    explicit sleepDialog(QWidget *parent = nullptr);
    ~sleepDialog();

    void setcurrentsleep(const QString &csleep);
    void setcurrentscreen(const QString &cscreen);
    void setdevicelabel(const QString &description);
    void setandroidlabel(const QString &android);

    QString sleepValue();
    QString screenValue();

private slots:
    void on_sleepOff_clicked();
    void on_sleepOn_clicked();
    void on_screenOff_clicked();
    void on_screenOn_clicked();

private:
    int m_andos;
    QLabel *m_title;
    QLabel *m_deviceLabel;
    QLabel *m_androidLabel;
    QRadioButton *m_sleepOff;
    QRadioButton *m_sleepOn;
    QLineEdit *m_sleepEdit;
    QRadioButton *m_screenOff;
    QRadioButton *m_screenOn;
    QLineEdit *m_screenEdit;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
};

#endif // SLEEPDIALOG_H
