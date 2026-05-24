#include "oculusdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

int xandos;
bool guardian;
bool proximity;
bool screentimer;

oculusDialog::oculusDialog(QWidget *parent) :
    QDialog(parent)
{
    setFixedSize(581, 602);
    setWindowTitle("Quest Settings");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 10, 50, 10);
    mainLayout->setSpacing(8);

    title = new QLabel("Quest Headset", this);
    mainLayout->addWidget(title);

    battery1Label = new QLabel("battery1", this);
    battery1Label->setFixedSize(500, 16);
    battery1Label->setMinimumSize(300, 16);
    mainLayout->addWidget(battery1Label);

    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(5);

    auto makeLabel = [](const QString &text, int fixedW, int fixedH) {
        QLabel *lbl = new QLabel(text);
        if (fixedH > 0)
            lbl->setFixedSize(fixedW, fixedH);
        else
            lbl->setFixedWidth(fixedW);
        return lbl;
    };

    auto makeCombo = [](const QStringList &items, const QString &toolTip) {
        QComboBox *cb = new QComboBox();
        cb->setFixedWidth(105);
        cb->addItems(items);
        cb->setToolTip(toolTip);
        return cb;
    };

    proximityBox = makeCombo({"Choose", "Enable", "Disable"},
        "The proximity sensor is used to detect headset movement. Deactivating this sensor prevents the headset from sleeping.");
    grid->addWidget(makeLabel("Proximity sensor", 130, 16), 0, 0);
    grid->addWidget(proximityBox, 0, 1);

    foveation2Box = makeCombo({"Choose", "0", "1", "2", "3", "4"},
        "Override the rendering quality in the outer edges of the display. Higher is better performance, lower is better quality.");
    grid->addWidget(makeLabel("Foveation level", 130, 16), 0, 2);
    grid->addWidget(foveation2Box, 0, 3);

    guardianBox = makeCombo({"Choose", "Enable", "Disable"},
        "Used to disable the guardian boundary system");
    grid->addWidget(makeLabel("Guardian", 130, 16), 1, 0);
    grid->addWidget(guardianBox, 1, 1);

    foveation1Box = makeCombo({"Choose", "Enable", "Disable"},
        "Set to disabled to test an app with a fixed foveation level.");
    grid->addWidget(makeLabel("Dynamic Foveation", 130, 16), 1, 2);
    grid->addWidget(foveation1Box, 1, 3);

    powerBox = makeCombo({"Choose", "False", "True", "USB", "AC", "Wireless"},
        "Change sleep status depending on connection type: USB, AC, or wireless. True enables for all types, false disables for all types.");
    grid->addWidget(makeLabel("Power status", 130, 0), 2, 0);
    grid->addWidget(powerBox, 2, 1);

    chromaticBox = makeCombo({"Choose", "Enable", "Disable"},
        "Chromatic aberration counters unwanted colors at the edges of objects caused by the lenses. Enabled by default.");
    grid->addWidget(makeLabel("Chromatic aberration", 130, 16), 2, 2);
    grid->addWidget(chromaticBox, 2, 3);

    refreshBox = makeCombo({"Choose", "72", "90", "120"},
        "Override the default refresh rate of the screen.");
    grid->addWidget(makeLabel("Refresh rate", 130, 0), 3, 0);
    grid->addWidget(refreshBox, 3, 1);

    textureBox = makeCombo({"Choose", "1536p", "1440p"},
        "Change the default resolution setting at which the Quest headset renders apps");
    textureBox->setEditable(false);
    grid->addWidget(makeLabel("Texture resolution", 130, 0), 3, 2);
    grid->addWidget(textureBox, 3, 3);

    cpulevelBox = makeCombo({"Choose", "0", "1", "2", "3", "4"},
        "Override the default CPU level.");
    grid->addWidget(makeLabel("CPU level", 130, 0), 4, 0);
    grid->addWidget(cpulevelBox, 4, 1);

    resolutionBox = makeCombo({"Choose", "480p", "720p", "1080p", "Default"},
        "Override the standard 1024x1024 recording resolution.");
    grid->addWidget(makeLabel("Capture resolution", 130, 0), 4, 2);
    grid->addWidget(resolutionBox, 4, 3);

    gpulevelBox = makeCombo({"Choose", "0", "1", "2", "3", "4"},
        "Override the default GPU level.");
    grid->addWidget(makeLabel("GPU level", 130, 0), 5, 0);
    grid->addWidget(gpulevelBox, 5, 1);

    ratecaptureBox = makeCombo({"Choose", "Enable", "Disable"},
        "Used to enable / disable video capture at full refresh rate. It is half by default.");
    grid->addWidget(makeLabel("Full rate capture", 130, 16), 5, 2);
    grid->addWidget(ratecaptureBox, 5, 3);

    experBox = makeCombo({"Choose", "Enable", "Disable"},
        "Set experimental mode, allowing access to functions that may otherwise be inaccessible.");
    experBox->setEditable(false);
    grid->addWidget(makeLabel("Experimental mode", 130, 0), 6, 0);
    grid->addWidget(experBox, 6, 1);

    QLabel *label_25 = new QLabel("Capture Bitrate");
    label_25->setFixedWidth(100);
    grid->addWidget(label_25, 6, 2);

    captureBitrate = new QLineEdit();
    captureBitrate->setFixedWidth(105);
    captureBitrate->setClearButtonEnabled(true);
    captureBitrate->setFocusPolicy(Qt::ClickFocus);
    captureBitrate->setToolTip("Override the default capture bitrate.");
    grid->addWidget(captureBitrate, 6, 3);

    mainLayout->addLayout(grid);

    QHBoxLayout *groupBoxLayout = new QHBoxLayout();
    groupBoxLayout->setSpacing(10);

    QGroupBox *customResGroup = new QGroupBox("Custom resolutions");
    QGridLayout *customResGrid = new QGridLayout(customResGroup);

    auto makeResLabel = [](const QString &text) {
        QLabel *lbl = new QLabel(text);
        lbl->setFixedWidth(100);
        return lbl;
    };

    auto makeLineEdit = [](const QString &toolTip) {
        QLineEdit *le = new QLineEdit();
        le->setFixedWidth(105);
        le->setClearButtonEnabled(true);
        le->setFocusPolicy(Qt::ClickFocus);
        le->setToolTip(toolTip);
        return le;
    };

    textureWidth = makeLineEdit("Override the default texture resolution width");
    customResGrid->addWidget(makeResLabel("Texture Width"), 0, 0);
    customResGrid->addWidget(textureWidth, 0, 1);

    textureHeight = makeLineEdit("Override the default texture resolution height");
    customResGrid->addWidget(makeResLabel("Texture Height"), 1, 0);
    customResGrid->addWidget(textureHeight, 1, 1);

    captureWidth = makeLineEdit("Override the default capture resolution width");
    customResGrid->addWidget(makeResLabel("Capture Width"), 2, 0);
    customResGrid->addWidget(captureWidth, 2, 1);

    captureHeight = makeLineEdit("Override the default capture resolution height");
    customResGrid->addWidget(makeResLabel("Capture Height"), 3, 0);
    customResGrid->addWidget(captureHeight, 3, 1);

    groupBoxLayout->addWidget(customResGroup);

    QGroupBox *fovGroup = new QGroupBox("Field of View (Degrees)");
    QGridLayout *fovGrid = new QGridLayout(fovGroup);

    auto makeFovLabel = [](const QString &text) {
        QLabel *lbl = new QLabel(text);
        lbl->setFixedWidth(70);
        return lbl;
    };

    fovdown = makeLineEdit("Field of view downward in degrees");
    fovGrid->addWidget(makeFovLabel("Down"), 0, 0);
    fovGrid->addWidget(fovdown, 0, 1);

    fovup = makeLineEdit("Field of view upward in degrees");
    fovGrid->addWidget(makeFovLabel("Up"), 1, 0);
    fovGrid->addWidget(fovup, 1, 1);

    fovin = makeLineEdit("Field of view inward in degrees");
    fovGrid->addWidget(makeFovLabel("Inward"), 2, 0);
    fovGrid->addWidget(fovin, 2, 1);

    fovout = makeLineEdit("Field of view outward in degrees");
    fovGrid->addWidget(makeFovLabel("Outward"), 3, 0);
    fovGrid->addWidget(fovout, 3, 1);

    groupBoxLayout->addWidget(fovGroup);

    mainLayout->addLayout(groupBoxLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    cancelButton = new QPushButton("Cancel");
    cancelButton->setToolTip("Close dialog without taking any action.");
    cancelButton->setAutoDefault(false);
    cancelButton->setDefault(true);

    saveButton = new QPushButton("Save");
    saveButton->setToolTip("Close dialog and save settings without applying.");

    okButton = new QPushButton("OK");
    okButton->setToolTip("Save settings and apply to device");

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(okButton);
    mainLayout->addLayout(buttonLayout);

    connect(cancelButton, &QPushButton::clicked, this, &oculusDialog::on_cancelButton_clicked);
    connect(saveButton, &QPushButton::clicked, this, &oculusDialog::on_saveButton_clicked);
    connect(okButton, &QPushButton::clicked, this, &oculusDialog::on_okButton_clicked);
}

oculusDialog::~oculusDialog()
{
}

void oculusDialog::titleSet(QString title) {
    this->title->setText(title);
}

void oculusDialog::setbattery1label(const QString &b1)
{
    battery1Label->setText(b1);
}

int  oculusDialog::textureSelected() const {
    return textureBox->currentIndex();
}

void oculusDialog::textureSet(int index) {
    textureBox->setCurrentIndex(index);
}

QString oculusDialog::fovdownSelected() const {
    return fovdown->text();
}

void oculusDialog::fovdownSet(QString fovdown) {
    this->fovdown->setText(fovdown);
}

void oculusDialog::fovupSet(QString fovup) {
    this->fovup->setText(fovup);
}

QString oculusDialog::fovupSelected() const {
    return fovup->text();
}

void oculusDialog::fovoutSet(QString fovout) {
    this->fovout->setText(fovout);
}

QString oculusDialog::fovoutSelected() const {
    return fovout->text();
}

QString oculusDialog::fovinSelected() const {
    return fovin->text();
}

void oculusDialog::fovinSet(QString fovin) {
    this->fovin->setText(fovin);
}

QString oculusDialog::customCaptureWidthSelected() const {
    return captureWidth->text();
}

void oculusDialog::customCaptureWidthSet(QString cwidth) {
   captureWidth->setText(cwidth);
}

QString oculusDialog::customCaptureHeightSelected() const {
   return captureHeight->text();
}

void oculusDialog::customCaptureHeightSet(QString cwidth) {
   captureHeight->setText(cwidth);
}

QString oculusDialog::customTextureWidthSelected() const {
   return textureWidth->text();
}

void oculusDialog::customTextureWidthSet(QString twidth) {
   textureWidth->setText(twidth);
}

QString oculusDialog::customTextureHeightSelected() const {
   return textureHeight->text();
}

void oculusDialog::customTextureHeightSet(QString theight) {
   textureHeight->setText(theight);
}

void oculusDialog::bitrateSet(QString bitrate) {
   captureBitrate->setText(bitrate);
}

QString oculusDialog::bitrateSelected() const {
   return captureBitrate->text();
}

void oculusDialog::chromaticSet(int index) {
    chromaticBox->setCurrentIndex(index);
}

int  oculusDialog::chromaticSelected() const {
    return chromaticBox->currentIndex();
}

void oculusDialog::recordingSet(int index) {
    resolutionBox->setCurrentIndex(index);
}

int oculusDialog::recordingSelected() const {
    return resolutionBox->currentIndex();
}

void oculusDialog::ratecapSet(int index) {
    ratecaptureBox->setCurrentIndex(index);
}

int  oculusDialog::ratecapSelected() const {
    return ratecaptureBox->currentIndex();
}

int  oculusDialog::dynamicfovSelected() const {
    return foveation2Box->currentIndex();
}

void oculusDialog::dynamicfovSet(int index) {
    foveation2Box->setCurrentIndex(index);
}

void oculusDialog::fovlevelSet(int index) {
    foveation1Box->setCurrentIndex(index);
}

int  oculusDialog::fovlevelSelected() const {
    return foveation1Box->currentIndex();
}

void oculusDialog::powerSet(int index) {
    powerBox->setCurrentIndex(index);
}

int oculusDialog::powerSelected() const {
    return powerBox->currentIndex();
}

void oculusDialog::proximitySet(int index) {
    proximityBox->setCurrentIndex(index);
}

int oculusDialog::proximitySelected() const {
    return proximityBox->currentIndex();
}

void oculusDialog::guardianSet(int index) {
    guardianBox->setCurrentIndex(index);
}

int oculusDialog::guardianSelected() const {
    return guardianBox->currentIndex();
}

void oculusDialog::cpuSet(int index) {
    cpulevelBox->setCurrentIndex(index);
}

int oculusDialog::cpuSelected() const {
    return cpulevelBox->currentIndex();
}

int oculusDialog::gpuSelected() const {
    return gpulevelBox->currentIndex();
}

void oculusDialog::gpuSet(int index) {
    gpulevelBox->setCurrentIndex(index);
}

void oculusDialog::refreshSet(int index) {
    refreshBox->setCurrentIndex(index);
}

int oculusDialog::refreshSelected() const {
    return refreshBox->currentIndex();
}

void oculusDialog::experSet(int index) {
    experBox->setCurrentIndex(index);
}

int oculusDialog::experSelected() const {
    return experBox->currentIndex();
}

void oculusDialog::on_cancelButton_clicked()
{
    clickedButton=2;
    reject();
}

void oculusDialog::on_saveButton_clicked()
{
    clickedButton=1;
    accept();
}

void oculusDialog::on_okButton_clicked()
{
    clickedButton=0;
    accept();
}
