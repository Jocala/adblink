#include "datadialog.h"
#include <QStringList>
#include <QMessageBox>

int dataDialog::returnval2() {
    return m_rval2;
}

QString dataDialog::externalLocation() {
    return m_externalLocation;
}

dataDialog::dataDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Move Kodi Data");
    setFixedSize(325, 325);

    m_labelSelectPartition = new QLabel("Select partition", this);
    m_labelSelectPartition->setGeometry(100, 40, 111, 16);

    m_listDirectories = new QListWidget(this);
    m_listDirectories->setGeometry(59, 70, 180, 100);
    m_listDirectories->setMinimumSize(180, 100);
    m_listDirectories->setMaximumSize(180, 100);
    m_listDirectories->setToolTip("Click to select an external partition");
    m_listDirectories->setStyleSheet(
        "QListWidget {\n"
        "    border: 2px solid #8f8f91;\n"
        "    border-radius: 6px;\n"
        "    border-color: black;\n"
        "}");

    m_labelChooseOperation = new QLabel("Choose operation below", this);
    m_labelChooseOperation->setGeometry(70, 180, 161, 16);

    m_dataButton1 = new QRadioButton("Move data from /sdcard/ to selection", this);
    m_dataButton1->setGeometry(50, 203, 331, 20);
    m_dataButton1->setChecked(true);

    m_dataButton2 = new QRadioButton("Move data from selection to /sdcard/", this);
    m_dataButton2->setGeometry(50, 222, 331, 20);

//    m_dataButton3 = new QRadioButton("Move thumbnails from /sdcard to selection", this);
//    m_dataButton3->setGeometry(20, 241, 331, 20);
//
//    m_dataButton4 = new QRadioButton("Move thumbnails from selection to /sdcard", this);
//    m_dataButton4->setGeometry(20, 260, 291, 20);

    m_cancelButton = new QPushButton("Cancel", this);
    m_okButton = new QPushButton("OK", this);
    m_cancelButton->setGeometry(70, 285, 75, 32);
    m_okButton->setGeometry(155, 285, 75, 32);

    m_rval2 = 1;

    connect(m_dataButton1, &QRadioButton::clicked, this, [this]() { on_dataButton1_clicked(); });
    connect(m_dataButton2, &QRadioButton::clicked, this, [this]() { on_dataButton2_clicked(); });
//    connect(m_dataButton3, &QRadioButton::clicked, this, [this]() { on_dataButton3_clicked(); });
//    connect(m_dataButton4, &QRadioButton::clicked, this, [this]() { on_dataButton4_clicked(); });
    connect(m_okButton, &QPushButton::clicked, this, [this]() { on_okButton_clicked(); });
    connect(m_cancelButton, &QPushButton::clicked, this, [this]() { reject(); });
    connect(m_listDirectories, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *) { on_listDirectories1_doubleClicked(); });
}

dataDialog::~dataDialog()
{
}

void dataDialog::setadb_data(const QStringList &list)
{
    m_listDirectories->clear();
    for (int i = 0; i < list.size(); i++) {
        m_listDirectories->addItem(list[i]);
    }
    m_listDirectories->item(0)->setSelected(true);
}

void dataDialog::on_dataButton1_clicked()
{
    m_rval2 = 1;
}

void dataDialog::on_dataButton2_clicked()
{
    m_rval2 = 2;
}

//void dataDialog::on_dataButton3_clicked()
//{
//    m_rval2 = 3;
//}
//
//void dataDialog::on_dataButton4_clicked()
//{
//    m_rval2 = 4;
//}

void dataDialog::on_listDirectories1_doubleClicked()
{
    if (m_listDirectories->currentItem() != nullptr) {
        m_externalLocation = m_listDirectories->currentItem()->text();
    } else if (m_listDirectories->count() > 0) {
        m_listDirectories->setCurrentRow(0);
        m_externalLocation = m_listDirectories->currentItem()->text();
    } else {
        QMessageBox::information(this, "No Items", "No items available in the list.");
        return;
    }

    accept();
}

void dataDialog::on_okButton_clicked()
{
    on_listDirectories1_doubleClicked();
}
