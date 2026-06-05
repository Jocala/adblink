#include "restdialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

restDialog::restDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(230, 230);
    setWindowTitle(QStringLiteral("Restore"));

    auto *label = new QLabel(QStringLiteral("Choose Kodi partition"), this);
    label->setGeometry(50, 20, 141, 16);

    m_listDirectories = new QListWidget(this);
    m_listDirectories->setObjectName(QStringLiteral("listDirectories"));
    m_listDirectories->setGeometry(40, 60, 150, 100);
    m_listDirectories->setMinimumSize(150, 100);
    m_listDirectories->setMaximumSize(150, 100);
    m_listDirectories->setToolTip(QStringLiteral("<html><head/><body><p>Click Kodi&quot;s root partition. Default is /sdcard/</p></body></html>"));
    m_listDirectories->setStyleSheet(QStringLiteral(
        "QListWidget {"
        "    border: 2px solid #8f8f91;"
        "    border-radius: 6px;"
        "    border-color: black;"
        "}"
    ));

    auto *buttonWidget = new QWidget(this);
    buttonWidget->setGeometry(30, 180, 164, 32);
    auto *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    m_cancelButton = new QPushButton(QStringLiteral("Cancel"), buttonWidget);
    m_cancelButton->setObjectName(QStringLiteral("cancelButton"));
    buttonLayout->addWidget(m_cancelButton);

    m_okButton = new QPushButton(QStringLiteral("OK"), buttonWidget);
    m_okButton->setObjectName(QStringLiteral("okButton"));
    buttonLayout->addWidget(m_okButton);

    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_listDirectories, &QListWidget::itemDoubleClicked, this, &QDialog::accept);
}

restDialog::~restDialog()
{
}

void restDialog::setadb_restore(const QStringList &list)
{
    m_listDirectories->clear();
    for (int i = 0; i < list.size(); i++) {
        m_listDirectories->addItem(list[i]);
    }
    m_listDirectories->item(0)->setSelected(true);
}

QString restDialog::restore_data_root()
{
    if (m_listDirectories->currentItem() != nullptr) {
        return m_listDirectories->currentItem()->text();
    } else if (m_listDirectories->count() > 0) {
        m_listDirectories->setCurrentRow(0);
        return m_listDirectories->currentItem()->text();
    } else {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QStringLiteral("No Items"));
        msgBox.setText(QStringLiteral("No items available in the list."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return QString();
    }
}

void restDialog::on_listDirectories_doubleClicked()
{
    if (m_listDirectories->currentItem() != nullptr) {
        restDialog::accept();
    } else if (m_listDirectories->count() > 0) {
        m_listDirectories->setCurrentRow(0);
        restDialog::accept();
    } else {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QStringLiteral("No Items"));
        msgBox.setText(QStringLiteral("No items available in the list."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
    }
}

void restDialog::on_okButton_clicked()
{
    on_listDirectories_doubleClicked();
}
