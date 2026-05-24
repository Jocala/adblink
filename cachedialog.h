#ifndef CACHEDIALOG_H
#define CACHEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>

class cacheDialog : public QDialog
{
    Q_OBJECT

public:
    explicit cacheDialog(QWidget *parent = 0);
    ~cacheDialog();

    int cbuffermode();
    QString cbuffersize();
    QString cbufferfactor();
    bool cacheclip();

    void setcbuffermode(const int &cbuffermode);
    void setcbuffersize(const QString &cbuffersize);
    void setcbufferfactor(const QString &cbufferfactor);

    int returncval1();

    void setxmlcheck(const bool &xmlcheck);
    bool xmlcheck();

private slots:
    void on_cpreset1_clicked();
    void on_cpreset2_clicked();
    void on_cpreset3_clicked();
    void on_writecxml_clicked();
    void on_resetcxml_clicked();
    void on_copyxml_clicked();

private:
    QLabel *titleLabel;
    QComboBox *buffermode;
    QLineEdit *buffersize;
    QLineEdit *bufferfactor;
    QCheckBox *xmlcheckBox;
    QRadioButton *cpreset1;
    QRadioButton *cpreset2;
    QRadioButton *cpreset3;
    QPushButton *writecxml;
    QPushButton *copyxml;
    QPushButton *resetcxml;
    QPushButton *cancelButton;
};

extern QClipboard *cacheclipboard;
extern int cval1;
extern QString cachecontent;

#endif // CACHEDIALOG_H
