#ifndef TCPIPDIALOG_H
#define TCPIPDIALOG_H

#include <QDialog>

class QLabel;
class QPushButton;

class tcpipDialog : public QDialog
{
    Q_OBJECT

public:
    explicit tcpipDialog(QWidget *parent = nullptr);
    ~tcpipDialog();

    int returntcp();
    void settcplabel(const QString &labeltext);

private slots:
    void on_tcpipOn_clicked();

private:
    int m_tcpon;
    QLabel *m_tcplabel;
    QPushButton *m_tcpipCancel;
    QPushButton *m_tcpipOn;
};

#endif // TCPIPDIALOG_H
