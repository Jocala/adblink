#include "pairdialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QVBoxLayout>

PairDialog::PairDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Pair device (Android 11+)"));
    setMinimumWidth(460);

    auto *intro = new QLabel(this);
    intro->setWordWrap(true);
    intro->setText(QStringLiteral(
        "For devices that use <b>Wireless debugging</b> (Android 11 and later, "
        "e.g. Google TV, Chromecast with Google TV, phones).<br><br>"
        "On the device open <i>Settings &gt; Developer options &gt; Wireless debugging</i>, "
        "then tap <i>Pair device with pairing code</i>. Keep that popup open while pairing.<br><br>"
        "Fire TV devices don't need this - use ADB debugging and Connect as usual."));

    m_host = new QLineEdit(this);
    m_host->setPlaceholderText(QStringLiteral("192.168.1.20"));

    m_pairPort = new QLineEdit(this);
    m_pairPort->setPlaceholderText(QStringLiteral("from the pairing popup, e.g. 37099"));
    m_pairPort->setValidator(new QIntValidator(1, 65535, m_pairPort));

    m_code = new QLineEdit(this);
    m_code->setPlaceholderText(QStringLiteral("6 digits"));
    m_code->setValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral("\\d{0,6}")), m_code));

    m_connectPort = new QLineEdit(this);
    m_connectPort->setPlaceholderText(QStringLiteral("optional: from the main Wireless debugging screen"));
    m_connectPort->setValidator(new QIntValidator(1, 65535, m_connectPort));

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("IP address:"), m_host);
    form->addRow(QStringLiteral("Pairing port:"), m_pairPort);
    form->addRow(QStringLiteral("Pairing code:"), m_code);
    form->addRow(QStringLiteral("Connect port:"), m_connectPort);

    m_error = new QLabel(this);
    m_error->setWordWrap(true);
    m_error->setStyleSheet(QStringLiteral("color: #c0392b;"));
    m_error->hide();

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    QPushButton *pair = buttons->addButton(QStringLiteral("Pair"), QDialogButtonBox::AcceptRole);
    pair->setDefault(true);
    connect(buttons, &QDialogButtonBox::accepted, this, &PairDialog::tryAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(intro);
    layout->addSpacing(8);
    layout->addLayout(form);
    layout->addWidget(m_error);
    layout->addWidget(buttons);
}

void PairDialog::setHost(const QString &host)
{
    m_host->setText(host);
    if (!host.isEmpty())
        m_pairPort->setFocus();
}

WirelessPairing::Request PairDialog::request() const
{
    WirelessPairing::Request r;
    r.host = m_host->text().trimmed();
    r.pairPort = m_pairPort->text().trimmed();
    r.code = m_code->text().trimmed();
    r.connectPort = m_connectPort->text().trimmed();
    return r;
}

void PairDialog::tryAccept()
{
    const QString error = WirelessPairing::validate(request());
    if (!error.isEmpty()) {
        m_error->setText(error);
        m_error->show();
        return;
    }
    accept();
}
