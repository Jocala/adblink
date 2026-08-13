#include "restoreoptionsdialog.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

RestoreOptionsDialog::RestoreOptionsDialog(const QString &deviceName, const QStringList &selected,
                                           QWidget *parent)
    : QDialog(parent)
{
    m_fileNames = {
        QStringLiteral("guisettings.xml"),
        QStringLiteral("advancedsettings.xml"),
        QStringLiteral("sources.xml"),
        QStringLiteral("favourites.xml"),
        QStringLiteral("profiles.xml"),
        QStringLiteral("RssFeeds.xml"),
        QStringLiteral("mediasources.xml"),
        QStringLiteral("passwords.xml"),
        QStringLiteral("Lircmap.xml"),
    };

    setWindowTitle(QStringLiteral("Restore Options"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(6);

    auto *label = new QLabel(QStringLiteral("Restore this backup to %1?\nThis will overwrite existing Kodi data.\nCheck files to preserve:").arg(deviceName), this);
    label->setWordWrap(true);
    layout->addWidget(label);

    auto *grid = new QGridLayout();
    grid->setSpacing(6);
    for (int i = 0; i < m_fileNames.size() && i < 9; i++) {
        m_fileChecks[i] = new QCheckBox(m_fileNames.at(i), this);
        m_fileChecks[i]->setChecked(selected.contains(m_fileNames.at(i)));
        grid->addWidget(m_fileChecks[i], i / 3, i % 3);
    }
    layout->addLayout(grid);

    auto *buttonRow = new QHBoxLayout();
    auto *selectAllButton = new QPushButton(QStringLiteral("Select All"), this);
    auto *clearButton = new QPushButton(QStringLiteral("Clear"), this);
    buttonRow->addWidget(selectAllButton);
    buttonRow->addWidget(clearButton);
    buttonRow->addStretch();
    layout->addLayout(buttonRow);

    auto *dialogButtons = new QHBoxLayout();
    auto *cancelButton = new QPushButton(QStringLiteral("Cancel"), this);
    auto *restoreButton = new QPushButton(QStringLiteral("Restore"), this);
    restoreButton->setDefault(true);
    dialogButtons->addStretch();
    dialogButtons->addWidget(cancelButton);
    dialogButtons->addWidget(restoreButton);
    layout->addLayout(dialogButtons);

    connect(selectAllButton, &QPushButton::clicked, this, &RestoreOptionsDialog::selectAll);
    connect(clearButton, &QPushButton::clicked, this, &RestoreOptionsDialog::clearAll);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(restoreButton, &QPushButton::clicked, this, &QDialog::accept);
}

QStringList RestoreOptionsDialog::checkedFiles() const
{
    QStringList result;
    for (int i = 0; i < m_fileNames.size() && i < 9; i++) {
        if (m_fileChecks[i]->isChecked())
            result.append(m_fileNames.at(i));
    }
    return result;
}

void RestoreOptionsDialog::selectAll()
{
    for (int i = 0; i < m_fileNames.size() && i < 9; i++)
        m_fileChecks[i]->setChecked(true);
}

void RestoreOptionsDialog::clearAll()
{
    for (int i = 0; i < m_fileNames.size() && i < 9; i++)
        m_fileChecks[i]->setChecked(false);
}
