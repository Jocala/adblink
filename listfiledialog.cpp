#include "listfiledialog.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

listfileDialog::listfileDialog(QWidget *parent) :
    QDialog(parent)
{
    resize(344, 299);

    auto *mainLayout = new QVBoxLayout(this);

    m_flistWidget = new QListWidget(this);
    m_flistWidget->setObjectName(QStringLiteral("flistWidget"));
    mainLayout->addWidget(m_flistWidget, 1);

    auto *buttonLayout = new QHBoxLayout;

    m_cancelButton = new QPushButton(QStringLiteral("Cancel"), this);
    m_cancelButton->setObjectName(QStringLiteral("cancelButton"));
    buttonLayout->addWidget(m_cancelButton);

    m_selectButton = new QPushButton(QStringLiteral("Select"), this);
    m_selectButton->setObjectName(QStringLiteral("selectButton"));
    buttonLayout->addWidget(m_selectButton);

    mainLayout->addLayout(buttonLayout);

    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_selectButton, &QPushButton::clicked, this, [this]() {
        if (m_flistWidget->selectedItems().isEmpty())
            return;
        fitem = m_flistWidget->selectedItems().first()->text();
        accept();
    });
    connect(m_flistWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        fitem = item->text();
        accept();
    });
}

listfileDialog::~listfileDialog()
{
}

void listfileDialog::setFilelist(const QStringList &filelist)
{
    for (QStringList::const_iterator it = filelist.begin();
            it != filelist.end(); ++it)
    {
        m_flistWidget->addItem(*it);
    }
}

void listfileDialog::setDialogTitle(const QString &dtitle)
{
    setWindowTitle(dtitle);
}

QString listfileDialog::return_fitem() {
   return fitem;
}
