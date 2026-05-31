#include "keyboarddialog.h"
#include "adbutils.h"
#include "getadbdata.h"

#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>

QString cstring;
QStringList args;

keyboardDialog::keyboardDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowModality(Qt::ApplicationModal);
    setFixedSize(410, 410);
    setWindowTitle(QStringLiteral("Wifi/USB Remote"));
    setModal(true);

    setStyleSheet(QStringLiteral(
        "QPushButton {"
        "    border: 2px solid #8f8f91;"
        "    border-radius: 6px;"
        "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "                                      stop: 0 #f6f7fa, stop: 1 #dadbde);"
        "}"
        "QPushButton:pressed {"
        "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "                                      stop: 0 #dadbde, stop: 1 #f6f7fa);"
        "}"
        "QPushButton:flat {"
        "    border: none;"
        "}"
        "QPushButton:default {"
        "    border-color: navy;"
        "}"
        "QLineEdit {"
        "    padding: 1px;"
        "    border-style: solid;"
        "    border: 2px solid gray;"
        "    border-radius: 8px;"
        "}"
        "QComboBox {"
        "    border: 1px solid gray;"
        "    border-radius: 3px;"
        "    padding: 1px 18px 1px 3px;"
        "    min-width: 6em;"
        "}"
        "QComboBox:editable {"
        "    background: white;"
        "}"
        "QComboBox:!editable, QComboBox::drop-down:editable {"
        "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "                                      stop: 0 #f6f7fa, stop: 1 #dadbde);"
        "}"
        "QComboBox:!editable:on, QComboBox::drop-down:editable:on {"
        "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "                                      stop: 0 #f6f7fa, stop: 1 #dadbde);"
        "}"
        "QComboBox:on {"
        "    padding-top: 3px;"
        "    padding-left: 4px;"
        "}"
        "QComboBox::drop-down {"
        "    subcontrol-origin: padding;"
        "    subcontrol-position: top right;"
        "    width: 15px;"
        "    border-left-width: 1px;"
        "    border-left-color: darkgray;"
        "    border-left-style: solid;"
        "    border-top-right-radius: 3px;"
        "    border-bottom-right-radius: 3px;"
        "}"
        "QComboBox::down-arrow {"
        "    image: url(/usr/share/icons/crystalsvg/16x16/actions/1downarrow.png);"
        "}"
        "QComboBox::down-arrow:on {"
        "    top: 1px;"
        "    left: 1px;"
        "}"
        "QMessageBox {"
        "    border: 2px solid #8f8f91;"
        "    border-radius: 6px;"
        "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "                                      stop: 0 #f6f7fa, stop: 1 #dadbde);"
        "}"
    ));

    auto *layoutWidget = new QWidget(this);
    layoutWidget->setGeometry(30, 22, 360, 308);
    auto *grid = new QGridLayout(layoutWidget);
    grid->setContentsMargins(0, 0, 0, 0);

    auto makeButton = [&](const QString &name, const QString &text) -> QPushButton* {
        auto *btn = new QPushButton(text, layoutWidget);
        btn->setObjectName(name);
        btn->setMinimumSize(110, 60);
        QFont f = btn->font();
        f.setPointSize(12);
        btn->setFont(f);
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setAutoDefault(false);
        return btn;
    };

    auto *upButton = makeButton(QStringLiteral("upButton"), QStringLiteral("Up"));
    auto *downButton = makeButton(QStringLiteral("downButton"), QStringLiteral("Down"));
    auto *homeButton = makeButton(QStringLiteral("homeButton"), QStringLiteral("Home"));
    auto *leftButton = makeButton(QStringLiteral("leftButton"), QStringLiteral("Left"));
    auto *rightButton = makeButton(QStringLiteral("rightButton"), QStringLiteral("Right"));
    auto *backButton = makeButton(QStringLiteral("backButton"), QStringLiteral("Back"));
    auto *selectButton = makeButton(QStringLiteral("selectButton"), QStringLiteral("Select"));
    auto *menuButton = makeButton(QStringLiteral("menuButton"), QStringLiteral("Menu"));
    auto *powerButton = makeButton(QStringLiteral("powerButton"), QStringLiteral("Sleep"));
    auto *fastforwardButton = makeButton(QStringLiteral("fastforwardButton"), QStringLiteral("FFwd"));
    auto *rewindButton = makeButton(QStringLiteral("rewindButton"), QStringLiteral("Rewind"));
    auto *playpauseButton = makeButton(QStringLiteral("playpauseButton"), QStringLiteral("Play/Pause"));

    grid->addWidget(upButton, 0, 0);
    grid->addWidget(downButton, 0, 1);
    grid->addWidget(homeButton, 0, 2);
    grid->addWidget(leftButton, 1, 0);
    grid->addWidget(rightButton, 1, 1);
    grid->addWidget(backButton, 1, 2);
    grid->addWidget(selectButton, 2, 0);
    grid->addWidget(menuButton, 2, 1);
    grid->addWidget(powerButton, 2, 2);
    grid->addWidget(fastforwardButton, 3, 0);
    grid->addWidget(rewindButton, 3, 1);
    grid->addWidget(playpauseButton, 3, 2);

    m_closeButton = new QPushButton(QStringLiteral("Exit"), this);
    m_closeButton->setObjectName(QStringLiteral("closeButton"));
    m_closeButton->setGeometry(150, 340, 114, 60);
    m_closeButton->setMinimumSize(110, 60);
    QFont cf = m_closeButton->font();
    cf.setPointSize(12);
    m_closeButton->setFont(cf);
    m_closeButton->setFocusPolicy(Qt::NoFocus);

    m_daddressLabel = new QLabel(QStringLiteral("device"), this);
    m_daddressLabel->setObjectName(QStringLiteral("daddressLabel"));
    m_daddressLabel->setGeometry(200, 710, 191, 16);

    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::reject);
}

keyboardDialog::~keyboardDialog()
{
}

void keyboardDialog::setdaddr(const QString &daddress)
{
    cstring = getadbpath() + " -s " + daddress + " shell input keyevent ";
}

void keyboardDialog::on_upButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("19"));
}

void keyboardDialog::on_downButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("20"));
}

void keyboardDialog::on_leftButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("21"));
}

void keyboardDialog::on_rightButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("22"));
}

void keyboardDialog::on_homeButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("3"));
}

void keyboardDialog::on_selectButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("23"));
}

void keyboardDialog::on_menuButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("82"));
}

void keyboardDialog::on_backButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("4"));
}

void keyboardDialog::on_fastforwardButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("90"));
}

void keyboardDialog::on_rewindButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("89"));
}

void keyboardDialog::on_playpauseButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("85"));
}

void keyboardDialog::on_powerButton_clicked()
{
    getadbOutput(cstring + QStringLiteral("26"));
}
