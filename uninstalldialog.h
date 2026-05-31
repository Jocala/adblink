#ifndef UNINSTALLDIALOG_H
#define UNINSTALLDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QListWidget>

class uninstallDialog : public QDialog
{
    Q_OBJECT

public:
    explicit uninstallDialog(const QString &daddr, const QString &port, QWidget *parent = nullptr);
    ~uninstallDialog();

    QString packageName();
    bool keepBox();

private slots:
    void loadList();
    void loadBox();
    void on_applyButton_clicked();
    void on_apkclearButton_clicked();

private:
    QString m_daddr;
    QString m_port;
    QString m_lastOutput;

    QLabel *m_titleLabel;
    QLineEdit *m_lineEdit;
    QPushButton *m_applyButton;
    QPushButton *m_clearButton;
    QCheckBox *m_keepBox;
    QListWidget *m_unlistWidget;
    QPushButton *m_cancelButton;
    QPushButton *m_okButton;
};

#endif // UNINSTALLDIALOG_H
