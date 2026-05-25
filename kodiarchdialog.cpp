#include "kodiarchdialog.h"

#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QVBoxLayout>

KodiArchDialog::KodiArchDialog(const QString &version, QWidget *parent)
    : QDialog(parent)
    , m_version(version)
{
    setWindowTitle("Select Download Option");
    setFixedWidth(250);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *layout = new QVBoxLayout(this);

    auto *label = new QLabel("Kodi " + version, this);
    layout->addWidget(label, 0, Qt::AlignHCenter);

    m_group = new QButtonGroup(this);
    auto *optV7a = new QRadioButton("Download v7a (32Bit)", this);
    auto *optV8a = new QRadioButton("Download v8a (64Bit)", this);
    auto *optWebsite = new QRadioButton("Open website", this);
    optV7a->setChecked(true);
    m_group->addButton(optV7a, 0);
    m_group->addButton(optV8a, 1);
    m_group->addButton(optWebsite, 2);

    layout->addWidget(optV7a);
    layout->addWidget(optV8a);
    layout->addWidget(optWebsite);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->setCenterButtons(true);
    layout->addWidget(buttons, 0, Qt::AlignHCenter);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

int KodiArchDialog::selectedArch() const
{
    return m_group->checkedId();
}
