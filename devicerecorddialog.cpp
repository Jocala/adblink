#include "devicerecorddialog.h"
#include <QFileDialog>

DeviceRecordDialog::DeviceRecordDialog(QWidget *parent, bool showkodi) :
    QDialog(parent)
{
    setObjectName("DeviceRecordDialog");
    setWindowTitle("Device Record");
    setToolTip("Choose Kodi's operating system");
    setLayoutDirection(Qt::LeftToRight);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    this->setFixedHeight(280);
    this->setFixedWidth(540);

    // ---------- Layout containers ----------

    // Device info fields (gridLayout_2)
    QWidget *deviceInfoWidget = new QWidget(this);
    deviceInfoWidget->setGeometry(30, 20, 206, 123);
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
    portUsbWidget->setGeometry(30, 160, 221, 30);
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
    buttonWidget->setGeometry(30, 210, 145, 32);
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
    kodiFieldsWidget->setGeometry(286, 24, 233, 117);
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

    // ---------- Defaults ----------

    m_packagename->setText("org.xbmc.kodi");
    m_filepath->setText("/files/.kodi");
    m_data_root->setText("/sdcard/");

    // ---------- Connections ----------

    connect(m_saveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    connect(m_pfolderButton, &QPushButton::clicked, this, &DeviceRecordDialog::on_pfolderButton_clicked);
    connect(m_filepathButton, &QPushButton::clicked, this, &DeviceRecordDialog::on_filepathButton_clicked);

    connect(m_isusb, &QCheckBox::clicked, this, &DeviceRecordDialog::on_isusb_clicked);

    // ---------- Initial state ----------

    if (!showkodi) {
        this->setFixedWidth(275);
    }

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
    return false;
}

bool DeviceRecordDialog::scoped() {
    return false;
}

QString DeviceRecordDialog::ostype() {
    return QStringLiteral("0");
}

bool DeviceRecordDialog::disableroot() {
    return false;
}

void DeviceRecordDialog::setPackagename(const QString &packagename)
{
    m_packagename->setText(packagename);
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
}

void DeviceRecordDialog::setwsa(const bool &wsa)
{
}

void DeviceRecordDialog::setversionLabel(const QString &versiontext)
{
}

void DeviceRecordDialog::setostype(const QString &ostype)
{
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
}

void DeviceRecordDialog::setport(const QString &port)
{
    m_port->setText(port);
}

void DeviceRecordDialog::setdisableroot(const bool &disableroot)
{
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

void DeviceRecordDialog::on_isusb_clicked(bool checked)
{
    if (checked) {
        m_port->setText("");
    } else {
        m_port->setText("5555");
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
