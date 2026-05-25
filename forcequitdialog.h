#ifndef FORCEQUITDIALOG_H
#define FORCEQUITDIALOG_H

#include <QDialog>

class QLineEdit;
class QLabel;
class QRadioButton;
class QPushButton;

class forcequitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit forcequitDialog(bool takeaction, QString appname, QWidget *parent = nullptr);
    ~forcequitDialog();

    QString packagename();

private slots:
    void on_kodiButton_clicked();
    void on_spmcButton_clicked();
    void on_otherButton_clicked();
    void on_pushButton_clicked();

private:
    bool m_action;

    QLineEdit *m_packagename;
    QLabel *m_title;
    QLabel *m_explainLabel;
    QRadioButton *m_spmcButton;
    QRadioButton *m_kodiButton;
    QRadioButton *m_otherButton;
    QPushButton *m_fqcancelButton;
    QPushButton *m_pushButton;
    QPushButton *m_fqokButton;
};

#endif // FORCEQUITDIALOG_H
