#include "adbprefdialog.h"
#include "version.h"

#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFormLayout>

#include <QDebug>

#ifdef Q_OS_LINUX
static const int osp = 0;
#elif defined(Q_OS_WIN)
static const int osp = 1;
#elif defined(Q_OS_MAC)
static const int osp = 2;
#else
static const int osp = -1;
#endif

adbprefDialog::adbprefDialog(QWidget *parent)
    : QDialog(parent),
    m_networkManager(new QNetworkAccessManager(this))
{
    setupUiManual();
    this->setFixedSize(300, 500);

    // Platform-specific combo box visibility/positioning
    if (osp == 0) { // Linux
        linTermCombo->move(10, 150);
        macTermCombo->setVisible(false);
        macTermCombo->setEnabled(false);
    } else if (osp == 1) { // Windows
        macTermCombo->setVisible(false);
        macTermCombo->setEnabled(false);
        linTermCombo->setVisible(false);
        linTermCombo->setEnabled(false);
    } else if (osp == 2) { // macOS
        macTermCombo->move(10, 150);
        linTermCombo->setVisible(false);
        linTermCombo->setEnabled(false);
    }
}

adbprefDialog::~adbprefDialog()
{

}

void adbprefDialog::setupUiManual()
{
    this->setWindowTitle("Preferences");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(4);  // Uniform vertical spacing
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // --- Checkboxes ---
    versionCheckBox = new QCheckBox("Check for updates at program start", this);
    versionCheckBox->setObjectName("versioncheck");
    mainLayout->addWidget(versionCheckBox);

    scrcpyArgsCheckBox = new QCheckBox("Ask for Scrcpy arguments", this);
    scrcpyArgsCheckBox->setObjectName("scrcpyargs");
    mainLayout->addWidget(scrcpyArgsCheckBox);

    startViewCheckBox = new QCheckBox("Kodi view at startup", this);
    startViewCheckBox->setObjectName("startview");
    mainLayout->addWidget(startViewCheckBox);

    bypassDisconnectCheckBox = new QCheckBox("Bypass disconnect prompts", this);
    bypassDisconnectCheckBox->setObjectName("bypassdisconnect");
    mainLayout->addWidget(bypassDisconnectCheckBox);

    defaultWindowCombo = new QComboBox(this); // Changed to QComboBox
    defaultWindowCombo->setObjectName("defaultwindow");
    defaultWindowCombo->addItems({"Small", "Medium", "Large"});
    mainLayout->addWidget(defaultWindowCombo);

    QHBoxLayout *scaleRow = new QHBoxLayout;
    QLabel *scaleLabel = new QLabel("Default scale:", this);
    scaleRow->addWidget(scaleLabel);
    scaleRow->addWidget(defaultWindowCombo);
    mainLayout->addLayout(scaleRow);










    lgfontCombo = new QComboBox(this);
    lgfontCombo->setObjectName("lgfontselect");
    lgfontCombo->addItems({"14","16","18","20"});

    QHBoxLayout *lfontRow = new QHBoxLayout;
    QLabel *lfontLabel = new QLabel("Large scale font:", this);
    lfontRow->addWidget(lfontLabel);
    lfontRow->addWidget(lgfontCombo);
    mainLayout->addLayout(lfontRow);


    mdfontCombo = new QComboBox(this);
    mdfontCombo->setObjectName("mdfontselect");
    mdfontCombo->addItems({"12","14","16","18"});

    QHBoxLayout *mfontRow = new QHBoxLayout;
    QLabel *mfontLabel = new QLabel("Medium scale font:", this);
    mfontRow->addWidget(mfontLabel);
    mfontRow->addWidget(mdfontCombo);
    mfontRow->setSpacing(1);
    mainLayout->addLayout(mfontRow);

    smfontCombo = new QComboBox(this);
    smfontCombo->setObjectName("smfontselect");
    smfontCombo->addItems({"10","12","14","16"});

    QHBoxLayout *sfontRow = new QHBoxLayout;
    QLabel *sfontLabel = new QLabel("Small scale font:", this);
    sfontRow->addWidget(sfontLabel);
    sfontRow->addWidget(smfontCombo);
    sfontRow->setSpacing(1);
    mainLayout->addLayout(sfontRow);


    fmfontCombo = new QComboBox(this);
    fmfontCombo->setObjectName("smfontselect");
    fmfontCombo->addItems({"12","16","18","22"});

    QHBoxLayout *fmfontRow = new QHBoxLayout;
    QLabel *fmfontLabel = new QLabel("File Manager font:", this);
    fmfontRow->addWidget(fmfontLabel);
    fmfontRow->addWidget(fmfontCombo);
    fmfontRow->setSpacing(1);
    mainLayout->addLayout(fmfontRow);





    // --- Platform dropdowns ---
    macTermCombo = new QComboBox(this);
    macTermCombo->setObjectName("macTerm");
    macTermCombo->addItems({"macOS Terminal", "iTerm2 Terminal"});
    mainLayout->addWidget(macTermCombo);

    linTermCombo = new QComboBox(this);
    linTermCombo->setObjectName("linTerm");
    linTermCombo->addItems({"Gnome Terminal", "XFCE4 Terminal", "KDE Konsole"});
    mainLayout->addWidget(linTermCombo);


    // --- Button rows ---
    const int buttonWidth = 90;
    auto addRow = [&](QPushButton *&button, QLineEdit *&edit,
                      const QString &btnText, const QString &btnObjectName, const QString &btnTooltip,
                      const QString &editObjectName, const QString &editTooltip,
                      void (adbprefDialog::*slot)()) {
        QHBoxLayout *row = new QHBoxLayout();
        button = new QPushButton(btnText, this);
        button->setObjectName(btnObjectName);



        button->setToolTip(btnTooltip);
        button->setFixedWidth(buttonWidth);

        edit = new QLineEdit(this);
        edit->setObjectName(editObjectName);
        edit->setToolTip(editTooltip);

        row->addWidget(button);
        row->addWidget(edit);
        mainLayout->addLayout(row);

        connect(button, &QPushButton::clicked, this, slot);
    };

    addRow(downloadButton, downloadPathEdit, "Pull", "downloadButton", "Default folder for pulled files",
           "dfilepath", "Default folder for pulled files", &adbprefDialog::on_downloadButton_clicked);

    addRow(installButton, installPathEdit, "APK files", "installButton", "Default folder for APK files",
           "ifilepath", "Default folder for APK files", &adbprefDialog::on_installButton_clicked);

    addRow(backupButton, backupPathEdit, "Backup", "backupButton", "Default folder for backup sets",
           "bfilepath", "Default folder for backup sets", &adbprefDialog::on_backupButton_clicked);

    addRow(adbButton, localAdbEdit, "External ADB", "adbButton", "Custom ADB. Leave blank to use adblink's included adb",
           "localadb", "Custom ADB. Leave blank to use adblink's included adb", &adbprefDialog::on_adbButton_clicked);

    // --- Bottom button row ---
    QHBoxLayout *bottomButtons = new QHBoxLayout();
    cancelButton = new QPushButton("Cancel", this);
    cancelButton->setObjectName("cancelButton");
    checkButton = new QPushButton("Updates", this);
    checkButton->setObjectName("checkButton");
    okButton = new QPushButton("OK", this);
    okButton->setObjectName("okButton");

    bottomButtons->addWidget(cancelButton);
    bottomButtons->addWidget(checkButton);
    bottomButtons->addWidget(okButton);
    mainLayout->addLayout(bottomButtons);

    connect(cancelButton, &QPushButton::clicked, this, &adbprefDialog::reject);
    connect(okButton, &QPushButton::clicked, this, &adbprefDialog::accept);
    connect(checkButton, &QPushButton::clicked, this, &adbprefDialog::on_checkButton_clicked);
}

void adbprefDialog::on_checkButton_clicked()
{
    QNetworkRequest request(QUrl("https://www.jocala.com/version.txt"));
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &adbprefDialog::onRequestCompleted);
}

void adbprefDialog::onRequestCompleted()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QStringLiteral("Network error"));
        msgBox.setText(reply->errorString());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        reply->deleteLater();
        return;
    }

    QString newVersion = QString::fromUtf8(reply->readAll()).trimmed();
    if (version != newVersion) {
        QDialog dialog(this);
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        QLabel *messageLabel = new QLabel("adblink version " + newVersion + " is ready. Download?", &dialog);
        layout->addWidget(messageLabel);

        QDialogButtonBox *buttonBox = new QDialogButtonBox(&dialog);
        buttonBox->addButton("Yes", QDialogButtonBox::AcceptRole);
        buttonBox->addButton("No", QDialogButtonBox::RejectRole);
        buttonBox->addButton("Changelog", QDialogButtonBox::ActionRole);
        layout->addWidget(buttonBox);

        QObject::connect(buttonBox, &QDialogButtonBox::accepted, [&dialog]() {
            QDesktopServices::openUrl(QUrl("https://www.jocala.com"));
            dialog.accept();
        });
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
        QObject::connect(buttonBox, &QDialogButtonBox::clicked, [&](QAbstractButton *button) {
            if (buttonBox->buttonRole(button) == QDialogButtonBox::ActionRole) {
                QDesktopServices::openUrl(QUrl("https://jocala.com/changelog.txt"));
                dialog.close();
            }
        });

        dialog.exec();
    } else {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QStringLiteral("Update"));
        msgBox.setText(QStringLiteral("No adblink update available"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
    }

    reply->deleteLater();
}

void adbprefDialog::on_downloadButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select pull folder");
    if (!dir.isEmpty()) {
        downloadPathEdit->setText(dir);
    }
}

void adbprefDialog::on_installButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select install folder");
    if (!dir.isEmpty()) {
        installPathEdit->setText(dir);
    }
}

void adbprefDialog::on_backupButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select backup folder");
    if (!dir.isEmpty()) {
        backupPathEdit->setText(dir);
    }
}



void adbprefDialog::on_adbButton_clicked()
{
    QFileDialog dialog(this, "Select ADB executable");
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setDirectory(QDir::rootPath());
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);


    if (dialog.exec() == QDialog::Accepted) {
        QString adbFilePath = dialog.selectedFiles().value(0);

        QFileInfo adbInfo(adbFilePath);
        if (adbInfo.exists() && adbInfo.isFile()


            ) {
            QString adbDir = adbInfo.absolutePath();
            localAdbEdit->setText(adbDir);
            return;
        }
    }
}





void adbprefDialog::accept()
{
    QDialog::accept();
}

// --- Getters ---

QString adbprefDialog::downloaddir() const { return downloadPathEdit->text(); }
QString adbprefDialog::installdir() const { return installPathEdit->text(); }
QString adbprefDialog::backupdir() const { return backupPathEdit->text(); }
QString adbprefDialog::localadb() const { return localAdbEdit->text(); }

bool adbprefDialog::versioncheck() const { return versionCheckBox->isChecked(); }
bool adbprefDialog::scrcpyargs() const { return scrcpyArgsCheckBox->isChecked(); }
bool adbprefDialog::startview() const { return startViewCheckBox->isChecked(); }
bool adbprefDialog::bypassdisconnect() const { return bypassDisconnectCheckBox->isChecked(); }
int adbprefDialog::defaultwindow() const { return defaultWindowCombo->currentIndex(); }
int adbprefDialog::linterm() const { return linTermCombo->currentIndex(); }
int adbprefDialog::macterm() const { return macTermCombo->currentIndex(); }

int adbprefDialog::fmfont() const { return fmfontCombo->currentIndex(); }
int adbprefDialog::lgfont() const { return lgfontCombo->currentIndex(); }
int adbprefDialog::mdfont() const { return mdfontCombo->currentIndex(); }
int adbprefDialog::smfont() const { return smfontCombo->currentIndex(); }

// --- Setters ---

void adbprefDialog::setdownloaddir(const QString &dir) { downloadPathEdit->setText(dir); }
void adbprefDialog::setinstalldir(const QString &dir) { installPathEdit->setText(dir); }
void adbprefDialog::setbackupdir(const QString &dir) { backupPathEdit->setText(dir); }
void adbprefDialog::setlocaladb(const QString &path) { localAdbEdit->setText(path); }

void adbprefDialog::setversioncheck(bool val) { versionCheckBox->setChecked(val); }
void adbprefDialog::setscrcpyargs(bool val) { scrcpyArgsCheckBox->setChecked(val); }
void adbprefDialog::setstartview(bool val) { startViewCheckBox->setChecked(val); }
void adbprefDialog::setbypassdisconnect(bool val) { bypassDisconnectCheckBox->setChecked(val); }
void adbprefDialog::setdefaultwindow(int index) { defaultWindowCombo->setCurrentIndex(index); }
void adbprefDialog::setlinterm(int index) { linTermCombo->setCurrentIndex(index); }
void adbprefDialog::setmacterm(int index) { macTermCombo->setCurrentIndex(index); }

void adbprefDialog::setfmfont(int index) { fmfontCombo->setCurrentIndex(index); }
void adbprefDialog::setlgfont(int index) { lgfontCombo->setCurrentIndex(index); }
void adbprefDialog::setmdfont(int index) { mdfontCombo->setCurrentIndex(index); }
void adbprefDialog::setsmfont(int index) { smfontCombo->setCurrentIndex(index); }

