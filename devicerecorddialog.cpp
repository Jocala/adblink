#include "devicerecorddialog.h"
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include "getadbdata.h"

DeviceRecordDialog::DeviceRecordDialog(QWidget *parent, bool showkodi) :
    QDialog(parent)
{
    setObjectName("DeviceRecordDialog");
    setWindowTitle("Device Record");
    setToolTip("Choose Kodi's operating system");
    setLayoutDirection(Qt::LeftToRight);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    this->setFixedHeight(425);
    this->setFixedWidth(540);

    // ---------- Widgets ----------

    // Connected devices list
    QLabel *label_5 = new QLabel("Connected devices", this);
    label_5->setGeometry(30, 26, 115, 16);

    m_listDevicesp = new QListWidget(this);
    m_listDevicesp->setGeometry(30, 50, 150, 100);
    m_listDevicesp->setMinimumSize(150, 100);
    m_listDevicesp->setMaximumSize(150, 100);
    m_listDevicesp->setToolTip("Double-click to copy value to address field");

    // Media Center group box
    m_mediaBox = new QGroupBox("Media Center", this);
    m_mediaBox->setGeometry(30, 600, 120, 111);

    m_kodiButton = new QRadioButton("Kodi", m_mediaBox);
    m_kodiButton->setGeometry(10, 30, 53, 20);
    m_kodiButton->setToolTip("<html><head/><body><p>Kodi media center</p></body></html>");
    m_kodiButton->setChecked(true);

    m_spmcButton = new QRadioButton("SPMC", m_mediaBox);
    m_spmcButton->setGeometry(10, 49, 62, 20);
    m_spmcButton->setToolTip("<html><head/><body><p>SPMC media center</p></body></html>");

    m_otherButton = new QRadioButton("Other", m_mediaBox);
    m_otherButton->setGeometry(10, 68, 60, 20);
    m_otherButton->setToolTip("<html><head/><body><p>Fill in file path and package name</p></body></html>");

    // Scoped storage override
    m_scoped = new QCheckBox("SS Override", this);
    m_scoped->setEnabled(true);
    m_scoped->setGeometry(170, 610, 97, 20);
    m_scoped->setToolTip("<html><head/><body><p>Android 11 and later typically use scoped storage. "
                       "Check this to override scoped storage if your Android 11 (or greater) "
                       "device does not implement it.</p></body></html>");

    // Version label
    m_versionLabel = new QLabel("adblink version: 9999", this);
    m_versionLabel->setGeometry(180, 690, 142, 20);

    // WSA checkbox
    m_wsa = new QCheckBox("WSA", this);
    m_wsa->setEnabled(true);
    m_wsa->setGeometry(190, 640, 56, 24);
    m_wsa->setToolTip("<html><head/><body><p>Windows Subsystem for Android: changes IP to 127.0.0.1 "
                    "and port to <span style=\" color:#008000;\">58526</span></p></body></html>");

    // OS type combo
    m_ostypeBox = new QComboBox(this);
    m_ostypeBox->setGeometry(50, 510, 100, 20);
    m_ostypeBox->setMinimumSize(100, 20);
    m_ostypeBox->setMaximumSize(100, 20);
    m_ostypeBox->setToolTip("Choose Kodi's operating system");
    m_ostypeBox->addItem("Android");
    m_ostypeBox->addItem("Windows");
    m_ostypeBox->addItem("macOS");
    m_ostypeBox->addItem("Linux");

    // Kodi root list
    m_listkodirootBox = new QListWidget(this);
    m_listkodirootBox->setGeometry(350, 610, 150, 100);
    m_listkodirootBox->setMinimumSize(150, 100);
    m_listkodirootBox->setMaximumSize(150, 100);

    // Disable root checkbox
    m_disableroot = new QCheckBox("Disable root", this);
    m_disableroot->setEnabled(true);
    m_disableroot->setGeometry(20, 550, 97, 20);
    m_disableroot->setToolTip("<html><head/><body><p>Disable root for misbehaving su installations</p></body></html>");

    // ---------- Layout containers ----------

    // Device info fields (gridLayout_2)
    QWidget *deviceInfoWidget = new QWidget(this);
    deviceInfoWidget->setGeometry(30, 160, 206, 123);
    QGridLayout *gridLayout_2 = new QGridLayout(deviceInfoWidget);
    gridLayout_2->setContentsMargins(0, 0, 0, 0);

    QLabel *devlab = new QLabel("Description");
    devlab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    devlab->setMinimumSize(74, 12);
    devlab->setMaximumSize(74, 16);

    m_description = new QLineEdit();
    m_description->setMinimumSize(120, 21);
    m_description->setMaximumSize(120, 21);
    m_description->setToolTip("<html><head/><body><p>Enter a general description for your device. </p></body></html>");

    QLabel *label_11 = new QLabel("IP Address");
    label_11->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label_11->setMinimumSize(74, 16);
    label_11->setMaximumSize(74, 16);
    label_11->setToolTip("<html><head/><body><p>Enter or Paste IP or USB Address</p></body></html>");

    m_daddr = new QLineEdit();
    m_daddr->setMinimumSize(120, 21);
    m_daddr->setMaximumSize(120, 21);
    m_daddr->setToolTip("<html><head/><body><p>Enter the serial number or IP address for your device. "
                      "You may also enter a device name if you have one set up with your router.</p></body></html>");

    QLabel *label_12 = new QLabel("Scrcpy arg");
    label_12->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label_12->setMinimumSize(74, 16);
    label_12->setMaximumSize(74, 16);
    label_12->setToolTip("<html><head/><body><p>Enter or Paste IP or USB Address</p></body></html>");

    m_scrcpy = new QLineEdit();
    m_scrcpy->setMinimumSize(120, 21);
    m_scrcpy->setMaximumSize(120, 21);
    m_scrcpy->setToolTip("Scrcpy argument for this device: e.g. --record c:\\file.mp4");

    m_pfolderButton = new QPushButton("Pull Folder");
    m_pfolderButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_pfolderButton->setMinimumSize(74, 32);
    m_pfolderButton->setMaximumSize(74, 32);
    m_pfolderButton->setToolTip("<html><head/><body><p>Override global folder for pulled files</p></body></html>");

    m_pulldir = new QLineEdit();
    m_pulldir->setMinimumSize(120, 21);
    m_pulldir->setMaximumSize(120, 21);
    m_pulldir->setToolTip("Pulled files are put in this folder");

    gridLayout_2->addWidget(devlab, 0, 0);
    gridLayout_2->addWidget(m_description, 0, 1);
    gridLayout_2->addWidget(label_11, 1, 0);
    gridLayout_2->addWidget(m_daddr, 1, 1);
    gridLayout_2->addWidget(label_12, 2, 0);
    gridLayout_2->addWidget(m_scrcpy, 2, 1);
    gridLayout_2->addWidget(m_pfolderButton, 3, 0);
    gridLayout_2->addWidget(m_pulldir, 3, 1);

    // Port / USB layout
    QWidget *portUsbWidget = new QWidget(this);
    portUsbWidget->setGeometry(30, 300, 221, 30);
    QHBoxLayout *horizontalLayout = new QHBoxLayout(portUsbWidget);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *horizontalLayout_3 = new QHBoxLayout();
    QLabel *label_6 = new QLabel("Port");
    label_6->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    m_port = new QLineEdit();
    m_port->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_port->setMinimumSize(50, 21);
    m_port->setMaximumSize(50, 21);
    m_port->setToolTip("<html><head/><body><p>Port for IP connections. Defaults to 5555</p></body></html>");
    m_port->setPlaceholderText("5555");

    horizontalLayout_3->addWidget(label_6);
    horizontalLayout_3->addWidget(m_port);

    m_isusb = new QCheckBox("USB Connection");
    m_isusb->setEnabled(true);
    m_isusb->setToolTip("<html><head/><body><p>Check this box for USB connections</p></body></html>");

    horizontalLayout->addLayout(horizontalLayout_3);
    horizontalLayout->addWidget(m_isusb);

    // Save / Cancel buttons
    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setGeometry(30, 350, 145, 32);
    QHBoxLayout *horizontalLayout_11 = new QHBoxLayout(buttonWidget);
    horizontalLayout_11->setContentsMargins(0, 0, 0, 0);

    m_saveButton = new QPushButton("Save");
    m_saveButton->setToolTip("<html><head/><body><p>Save data to database</p></body></html>");

    m_cancelButton = new QPushButton("Cancel");
    m_cancelButton->setToolTip("<html><head/><body><p>Close this dialog</p></body></html>");

    horizontalLayout_11->addWidget(m_saveButton);
    horizontalLayout_11->addWidget(m_cancelButton);

    // Kodi fields (gridLayout)
    QWidget *kodiFieldsWidget = new QWidget(this);
    kodiFieldsWidget->setGeometry(286, 164, 233, 117);
    QGridLayout *gridLayout = new QGridLayout(kodiFieldsWidget);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setHorizontalSpacing(5);
    gridLayout->setVerticalSpacing(0);

    m_kodi2 = new QLabel("Package name");

    m_packagename = new QLineEdit();
    m_packagename->setToolTip("<html><head/><body><p>Package name for your Media Center  software</p></body></html>");

    m_kodi3 = new QLabel("Kodi partition");

    m_data_root = new QLineEdit();

    m_filepathButton = new QPushButton("File Path");
    m_filepathButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_filepathButton->setMinimumSize(80, 32);
    m_filepathButton->setMaximumSize(80, 32);

    m_filepath = new QLineEdit();
    m_filepath->setToolTip("<html><head/><body><p>Enter the file path for your Media Center.  "
                         "This is combined with Package Name, e.g.  org.xbmc.kodi/files/.kodi</p></body></html>");

    gridLayout->addWidget(m_kodi2, 0, 0);
    gridLayout->addWidget(m_packagename, 0, 1);
    gridLayout->addWidget(m_kodi3, 1, 0);
    gridLayout->addWidget(m_data_root, 1, 1);
    gridLayout->addWidget(m_filepathButton, 2, 0);
    gridLayout->addWidget(m_filepath, 2, 1);

    // ---------- Connections ----------

    connect(m_saveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    connect(m_pfolderButton, &QPushButton::clicked, this, &DeviceRecordDialog::on_pfolderButton_clicked);
    connect(m_filepathButton, &QPushButton::clicked, this, &DeviceRecordDialog::on_filepathButton_clicked);

    connect(m_kodiButton, &QRadioButton::clicked, this, [this]() { on_kodiButton_clicked(); });
    connect(m_spmcButton, &QRadioButton::clicked, this, [this]() { on_spmcButton_clicked(); });
    connect(m_otherButton, &QRadioButton::clicked, this, [this]() { on_otherButton_clicked(); });

    connect(m_isusb, &QCheckBox::clicked, this, &DeviceRecordDialog::on_isusb_clicked);
    connect(m_wsa, &QCheckBox::clicked, this, &DeviceRecordDialog::on_wsa_clicked);
    connect(m_scoped, &QCheckBox::clicked, this, &DeviceRecordDialog::on_scoped_clicked);

    connect(m_ostypeBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DeviceRecordDialog::on_ostypeBox_currentIndexChanged);

    connect(m_listDevicesp, &QListWidget::doubleClicked,
            this, &DeviceRecordDialog::on_listDevicesp_doubleClicked);
    connect(m_listkodirootBox, &QListWidget::clicked,
            this, [this](const QModelIndex&) { on_listkodirootBox_clicked(); });

    // ---------- Initial state ----------

    if (!showkodi) {
        this->setFixedWidth(275);
    }

    m_ostypeBox->setVisible(showkodi);
    m_filepath->setVisible(showkodi);
    m_filepathButton->setVisible(showkodi);
    m_kodi2->setVisible(showkodi);
    m_kodi3->setVisible(showkodi);
    m_packagename->setVisible(showkodi);
    m_data_root->setVisible(showkodi);

    on_isusb_clicked(m_isusb->isChecked());
}

DeviceRecordDialog::~DeviceRecordDialog()
{
}

QString DeviceRecordDialog::xbmcpackageName() {
    return m_packagename->text();
}

QString DeviceRecordDialog::scrcpy() {
    return m_scrcpy->text();
}

QString DeviceRecordDialog::data_root() {
    return m_data_root->text();
}

QString DeviceRecordDialog::pulldir() {
    return m_pulldir->text();
}

QString DeviceRecordDialog::daddr() {
    return m_daddr->text();
}

QString DeviceRecordDialog::description() {
    return m_description->text();
}

QString DeviceRecordDialog::filepath() {
    return m_filepath->text();
}

QString DeviceRecordDialog::port() {
    return m_port->text();
}

bool DeviceRecordDialog::isusb() {
    return m_isusb->isChecked();
}

bool DeviceRecordDialog::wsa() {
    return m_wsa->isChecked();
}

bool DeviceRecordDialog::scoped() {
    return m_scoped->isChecked();
}

QString DeviceRecordDialog::ostype() {
    QString ost = QString::number(m_ostypeBox->currentIndex());
    return ost;
}

void DeviceRecordDialog::setPackagename(const QString &packagename)
{
    bool isset = false;
    m_packagename->setText(packagename);

    static const QString packagepreset1 = QStringLiteral("org.xbmc.kodi");
    static const QString packagepreset3 = QStringLiteral("com.semperpax.spmc16");

    if (packagename == packagepreset1) {
        m_kodiButton->setChecked(true);
        isset = true;
    }

    if (packagename == packagepreset3) {
        m_spmcButton->setChecked(true);
        isset = true;
    }

    if (!isset)
        m_otherButton->setChecked(true);
}

void DeviceRecordDialog::setPulldir(const QString &pulldir)
{
    m_pulldir->setText(pulldir);
}

void DeviceRecordDialog::setscrcpy(const QString &scrcpy)
{
    m_scrcpy->setText(scrcpy);
}

void DeviceRecordDialog::setisusb(const bool &isusb)
{
    m_isusb->setChecked(isusb);
}

void DeviceRecordDialog::setscope(const bool &scoped)
{
    m_scoped->setChecked(scoped);
}

void DeviceRecordDialog::setwsa(const bool &wsa)
{
    m_wsa->setChecked(wsa);
}

void DeviceRecordDialog::setversionLabel(const QString &versiontext)
{
    m_versionLabel->setText("adblink version: " + versiontext);
}

void DeviceRecordDialog::setostype(const QString &ostype)
{
    m_ostypeBox->setCurrentIndex(ostype.toInt());
}

void DeviceRecordDialog::setdevicelist(const QStringList &dstringlist)
{
    if (dstringlist.count() < 1)
        return;

    QString tmpstr;
    for (QStringList::const_iterator it = dstringlist.begin(); it != dstringlist.end(); ++it) {
        tmpstr = *it;
        m_listDevicesp->addItem(tmpstr);
    }
}

void DeviceRecordDialog::setdaddr(const QString &daddr)
{
    m_daddr->setText(daddr);
}

void DeviceRecordDialog::setdescription(const QString &description)
{
    m_description->setText(description);
}

void DeviceRecordDialog::setfilepath(const QString &filepath)
{
    m_filepath->setText(filepath);
}

void DeviceRecordDialog::setdataroot(const QString &data_root)
{
    m_data_root->setText(data_root);
    if (m_ostypeBox->currentIndex() == 0) {
    }
}

void DeviceRecordDialog::setport(const QString &port)
{
    m_port->setText(port);
}

void DeviceRecordDialog::on_pfolderButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose pull folder"),
                                                    m_pulldir->text(),
                                                    QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        m_pulldir->setText(dir);
    }
}

void DeviceRecordDialog::on_kodiButton_clicked()
{
    m_packagename->setText("org.xbmc.kodi");
    m_filepath->setText("/files/.kodi");
}

void DeviceRecordDialog::on_spmcButton_clicked()
{
    m_packagename->setText("com.semperpax.spmc16");
    m_filepath->setText("/files/.spmc");
}

void DeviceRecordDialog::on_otherButton_clicked()
{
    m_packagename->setText("");
    m_filepath->setText("");
}

void DeviceRecordDialog::on_isusb_clicked(bool checked)
{
    if (checked) {
        m_port->setText("");
    } else {
        m_port->setText("5555");
    }
}

void DeviceRecordDialog::setadb_pref(const QString &adb_pref)
{
    m_listkodirootBox->clear();
    QString command;
    QString cstring;

    if (ostype() == "0") {
        cstring = adb_pref + " shell /data/local/tmp/adblink/busybox find /storage -type d -maxdepth 1";
        QString s = getadbOutput(cstring);

        QStringList list = s.split('\n');
        for (int i = 0; i < list.size(); i++) {
            list[i].remove('\r');
            list[i].remove('\n');

            if (list[i] == "Android" ||
                list[i] == "Permission denied" ||
                list[i] == "/storage/emulated" ||
                list[i] == "/storage" ||
                list[i] == "/storage/self" ||
                list[i].contains("unknown") ||
                list[i].contains("not found")) {
                list.removeAt(i);
                i--;
            }
        }

        list.insert(0, "/sdcard");

        for (int i = 0; i < list.size(); i++) {
            m_listkodirootBox->addItem(list[i]);
        }

        m_listkodirootBox->item(0)->setSelected(true);
    }

    cstring = adb_pref + " shell /data/local/tmp/adblink/busybox which su";
    command = getadbOutput(cstring);
}

void DeviceRecordDialog::on_listkodirootBox_clicked()
{
    m_data_root->setText(m_listkodirootBox->currentItem()->text());
}

void DeviceRecordDialog::disable_ui()
{
    m_port->setText("");
    m_port->setEnabled(false);
    m_isusb->setEnabled(false);
    m_scoped->setEnabled(false);
    m_wsa->setEnabled(false);
    m_listkodirootBox->setEnabled(false);
    m_daddr->setText("");
    m_packagename->setText("");
    m_data_root->setText("");
    m_daddr->setEnabled(false);
    m_packagename->setEnabled(false);
    m_data_root->setEnabled(false);
    m_mediaBox->setDisabled(true);
}

void DeviceRecordDialog::enable_ui()
{
    m_port->setEnabled(true);
    m_port->setText("5555");
    m_isusb->setEnabled(true);
    m_scoped->setEnabled(true);
    m_wsa->setEnabled(true);
    m_listkodirootBox->setEnabled(true);
    m_daddr->setEnabled(true);
    m_packagename->setEnabled(true);
    m_data_root->setEnabled(true);
    m_mediaBox->setEnabled(true);
}

void DeviceRecordDialog::on_ostypeBox_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        enable_ui();
        if (m_kodiButton->isChecked()) {
            m_packagename->setText("org.xbmc.kodi");
            m_filepath->setText("/files/.kodi");
        }
        if (m_spmcButton->isChecked()) {
            m_packagename->setText("com.semperpax.spmc16");
            m_filepath->setText("/files/.spmc");
        }
        if (m_otherButton->isChecked()) {
            m_packagename->setText("");
            m_filepath->setText("");
        }
        if (m_data_root->text().isEmpty())
            m_data_root->setText("/sdcard/");
        m_daddr->setText("");
        break;

    case 1:
        m_daddr->setText("");
        m_packagename->setText("");
        m_data_root->setText("");
        { QString homedir = QDir::homePath();
        QString tmpdir1 = homedir + "/AppData/Roaming/Kodi";
        m_filepath->setText(tmpdir1); }
        disable_ui();
        break;

    case 2:
        { QString homedir = QDir::homePath();
        QString tmpdir1 = homedir + "/Library/Application Support/Kodi";
        m_daddr->setText("");
        m_packagename->setText("");
        m_data_root->setText("");
        m_filepath->setText(tmpdir1); }
        disable_ui();
        break;

    case 3:
        { QString homedir = QDir::homePath();
        QString tmpdir1 = homedir + "/.kodi";
        m_daddr->setText("");
        m_packagename->setText("");
        m_data_root->setText("");
        m_filepath->setText(tmpdir1); }
        disable_ui();
        break;

    case 4:
        enable_ui();
        m_daddr->setText("127.0.0.1");
        m_port->setText("58526");
        m_data_root->setText("/sdcard/");
        m_filepath->setText("kodi_data/.kodi");
        break;

    case 5:
        enable_ui();
        m_data_root->setText("/sdcard/");
        m_filepath->setText("kodi_data/files/.kodi");
        break;
    }
}

void DeviceRecordDialog::on_filepathButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose file path"),
                                                    m_filepath->text(),
                                                    QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        m_filepath->setText(dir);
    }
}

void DeviceRecordDialog::on_listDevicesp_doubleClicked(const QModelIndex &index)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "",
        QStringLiteral("Copy this address?"),
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
        return;
    m_daddr->setText(index.data(Qt::DisplayRole).toString());
}

bool DeviceRecordDialog::disableroot() {
    return m_disableroot->isChecked();
}

void DeviceRecordDialog::setdisableroot(const bool &disableroot)
{
    m_disableroot->setChecked(disableroot);
}

void DeviceRecordDialog::on_wsa_clicked(bool checked)
{
    enable_ui();
    if (checked) {
        m_daddr->setText("127.0.0.1");
        m_port->setText("58526");
        m_data_root->setText("/sdcard/");
        m_filepath->setText("kodi_data/files/.kodi");
        m_scoped->setChecked(true);
    } else {
        m_daddr->setText("");
        m_filepath->setText("/files/.kodi");
        m_port->setText("5555");
        m_data_root->setText("/sdcard/");
    }
}

void DeviceRecordDialog::on_scoped_clicked(bool checked)
{
    if (checked) {
        m_data_root->setText("/sdcard/");
        m_filepath->setText("kodi_data/files/.kodi");
    } else {
        m_filepath->setText("/files/.kodi");
        m_data_root->setText("/sdcard/");
    }
}

QString strip2(QString str)
{
    str = str.simplified();
    str.replace(" ", "");
    return str;
}
