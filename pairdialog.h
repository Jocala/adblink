#ifndef PAIRDIALOG_H
#define PAIRDIALOG_H

#include <QDialog>

#include "wirelesspairing.h"

class QLabel;
class QLineEdit;

// Collects the details for Android 11+ wireless debugging pairing.
class PairDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PairDialog(QWidget *parent = nullptr);

    void setHost(const QString &host);
    WirelessPairing::Request request() const;

private slots:
    void tryAccept();

private:
    QLineEdit *m_host;
    QLineEdit *m_pairPort;
    QLineEdit *m_code;
    QLineEdit *m_connectPort;
    QLabel *m_error;
};

#endif // PAIRDIALOG_H
