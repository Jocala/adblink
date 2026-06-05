#include "oculusmanager.h"
#include "oculusdialog.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QMessageBox>
#include <QDialog>

OculusManager::OculusManager(const QString &adbPrefix, const QString &dbDir, QWidget *parent)
    : m_adbPrefix(adbPrefix), m_dbDir(dbDir), m_parent(parent)
{
}

void OculusManager::exec()
{
    bool notQuest = false;

    QString cstring = m_adbPrefix + " shell dumpsys CompanionService | grep Battery";
    QString temp = ::getadbOutput(cstring);

    if (temp.contains("Can't find service: CompanionService")) {
        QMessageBox msgBox(m_parent);
        msgBox.setWindowTitle(QStringLiteral("Quest"));
        msgBox.setText(QStringLiteral("Not a Quest device. Proceed?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowModality(Qt::WindowModal);
        if (msgBox.exec() == QMessageBox::No)
            return;
        notQuest = true;
    }

    readOrCreateConfig();

    oculusDialog dialog(m_parent);
    dialog.setWindowModality(Qt::WindowModal);

    dialog.powerSet(m_power);
    dialog.proximitySet(m_proximity);
    dialog.guardianSet(m_guardian);
    dialog.cpuSet(m_cpu);
    dialog.gpuSet(m_gpu);
    dialog.refreshSet(m_refresh);
    dialog.experSet(m_exper);
    dialog.textureSet(m_texture);
    dialog.recordingSet(m_recording);
    dialog.fovlevelSet(m_fovlevel);
    dialog.dynamicfovSet(m_dynamicfov);
    dialog.ratecapSet(m_ratecap);
    dialog.chromaticSet(m_chromatic);
    dialog.customCaptureHeightSet(m_customCaptureHeight);
    dialog.customCaptureWidthSet(m_customCaptureWidth);
    dialog.customTextureHeightSet(m_customTextureHeight);
    dialog.customTextureWidthSet(m_customTextureWidth);
    dialog.bitrateSet(m_customBitrate);
    dialog.fovupSet(m_fovup);
    dialog.fovdownSet(m_fovdown);
    dialog.fovinSet(m_fovin);
    dialog.fovoutSet(m_fovout);

    int colonPos = temp.indexOf(":");
    QString b1;
    if (colonPos != -1)
        b1 = temp.mid(colonPos + 2).trimmed();
    else
        b1 = "";

    cstring = m_adbPrefix + " shell dumpsys OVRRemoteService | grep Battery";
    QString battery2 = ::getadbOutput(cstring);

    static QRegularExpression pattern("Battery:\\s*(\\d+)%");
    QRegularExpressionMatchIterator iterator = pattern.globalMatch(battery2);
    QString rightBattery, leftBattery;
    int matchCount = 0;
    while (iterator.hasNext() && matchCount < 2) {
        QRegularExpressionMatch match = iterator.next();
        QString batteryPercentage = match.captured(1);
        if (matchCount == 0)
            rightBattery = batteryPercentage;
        else
            leftBattery = batteryPercentage;
        matchCount++;
    }

    b1 = "Headset:" + b1 + "%  Controllers: L " + leftBattery + "% R " + rightBattery + "%";

    if (!notQuest) {
        dialog.titleSet("Quest Headset");
        dialog.setbattery1label("Batteries: " + b1);
    } else {
        dialog.titleSet("Unknown device");
        dialog.setbattery1label("Battery levels not found");
    }

    bool execute_true = false;

    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.clickedButton == 0)
            applySettings(dialog, execute_true);

        m_texture = dialog.textureSelected();
        m_recording = dialog.recordingSelected();
        m_fovlevel = dialog.fovlevelSelected();
        m_dynamicfov = dialog.dynamicfovSelected();
        m_ratecap = dialog.ratecapSelected();
        m_chromatic = dialog.chromaticSelected();
        m_proximity = dialog.proximitySelected();
        m_guardian = dialog.guardianSelected();
        m_exper = dialog.experSelected();
        m_power = dialog.powerSelected();
        m_cpu = dialog.cpuSelected();
        m_gpu = dialog.gpuSelected();
        m_refresh = dialog.refreshSelected();
        m_customCaptureWidth = dialog.customCaptureWidthSelected();
        m_customCaptureHeight = dialog.customCaptureHeightSelected();
        m_customTextureWidth = dialog.customTextureWidthSelected();
        m_customTextureHeight = dialog.customTextureHeightSelected();
        m_customBitrate = dialog.bitrateSelected();
        m_fovdown = dialog.fovdownSelected();
        m_fovup = dialog.fovupSelected();
        m_fovout = dialog.fovoutSelected();
        m_fovin = dialog.fovinSelected();

        saveConfig();
    }
}

void OculusManager::readOrCreateConfig()
{
    QString path = m_dbDir + "quest.json";

    if (!QFileInfo::exists(path)) {
        QJsonObject obj;
        obj["power"] = 0;
        obj["proximity"] = 0;
        obj["guardian"] = 0;
        obj["cpu"] = 0;
        obj["gpu"] = 0;
        obj["refresh"] = 0;
        obj["exper"] = 0;
        obj["texture"] = 0;
        obj["recording"] = 0;
        obj["fovlevel"] = 0;
        obj["dynamicfov"] = 0;
        obj["ratecap"] = 0;
        obj["chromatic"] = 0;
        obj["customCaptureHeight"] = "";
        obj["customCaptureWidth"] = "";
        obj["customTextureHeight"] = "";
        obj["customTextureWidth"] = "";
        obj["custombitrate"] = "";
        obj["fovdown"] = "";
        obj["fovup"] = "";
        obj["fovout"] = "";
        obj["fovin"] = "";

        QJsonDocument doc(obj);
        QFile file(path);
        (void)file.open(QIODevice::WriteOnly);
        file.write(doc.toJson());
        file.close();
    }

    QJsonObject obj;
    QJsonDocument doc;
    QFile file(path);
    (void)file.open(QIODevice::ReadOnly);
    doc = QJsonDocument::fromJson(file.readAll());
    obj = doc.object();
    file.close();

    m_power = obj["power"].toInt();
    m_proximity = obj["proximity"].toInt();
    m_guardian = obj["guardian"].toInt();
    m_cpu = obj["cpu"].toInt();
    m_gpu = obj["gpu"].toInt();
    m_exper = obj["exper"].toInt();
    m_refresh = obj["refresh"].toInt();
    m_texture = obj["texture"].toInt();
    m_recording = obj["recording"].toInt();
    m_fovlevel = obj["fovlevel"].toInt();
    m_dynamicfov = obj["dynamicfov"].toInt();
    m_ratecap = obj["ratecap"].toInt();
    m_chromatic = obj["chromatic"].toInt();
    m_customCaptureHeight = obj["customCaptureHeight"].toString();
    m_customCaptureWidth = obj["customCaptureWidth"].toString();
    m_customTextureHeight = obj["customTextureHeight"].toString();
    m_customTextureWidth = obj["customTextureWidth"].toString();
    m_customBitrate = obj["custombitrate"].toString();
    m_fovdown = obj["fovdown"].toString();
    m_fovup = obj["fovup"].toString();
    m_fovout = obj["fovout"].toString();
    m_fovin = obj["fovin"].toString();
}

void OculusManager::saveConfig()
{
    QJsonObject obj;
    obj["power"] = m_power;
    obj["proximity"] = m_proximity;
    obj["guardian"] = m_guardian;
    obj["cpu"] = m_cpu;
    obj["gpu"] = m_gpu;
    obj["exper"] = m_exper;
    obj["customCaptureWidth"] = m_customCaptureWidth;
    obj["customCaptureHeight"] = m_customCaptureHeight;
    obj["customTextureWidth"] = m_customTextureWidth;
    obj["customTextureHeight"] = m_customTextureHeight;
    obj["custombitrate"] = m_customBitrate;
    obj["fovdown"] = m_fovdown;
    obj["fovup"] = m_fovup;
    obj["fovout"] = m_fovout;
    obj["fovin"] = m_fovin;
    obj["refresh"] = m_refresh;
    obj["texture"] = m_texture;
    obj["recording"] = m_recording;
    obj["fovlevel"] = m_fovlevel;
    obj["dynamicfov"] = m_dynamicfov;
    obj["ratecap"] = m_ratecap;
    obj["chromatic"] = m_chromatic;

    QJsonDocument doc(obj);
    QFile file(m_dbDir + "quest.json");
    (void)file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.close();
}

void OculusManager::applySettings(oculusDialog &dialog, bool &execute_true)
{
    QString cstring;
    QString command;
    bool validInt;

    if (dialog.fovinSelected().toInt(&validInt) || dialog.fovinSelected() == "0") {
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.eyeFovInward  " + dialog.fovinSelected();
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
    }

    if (dialog.fovoutSelected().toInt(&validInt) || dialog.fovoutSelected() == "0") {
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.eyeFovOutward  " + dialog.fovoutSelected();
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
    }

    if (dialog.fovupSelected().toInt(&validInt) || dialog.fovupSelected() == "0") {
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.eyeFovUp  " + dialog.fovupSelected();
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
    }

    if (dialog.fovdownSelected().toInt(&validInt) || dialog.fovdownSelected() == "0") {
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.eyeFovDown  " + dialog.fovdownSelected();
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
    }

    if (dialog.bitrateSelected().toInt(&validInt) || dialog.bitrateSelected() == "0") {
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.capture.bitrate " + dialog.bitrateSelected();
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
    }

    if (dialog.customTextureHeightSelected().toInt(&validInt) && dialog.customTextureWidthSelected().toInt(&validInt)) {
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.textureWidth " + dialog.customTextureWidthSelected();
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        cstring = m_adbPrefix + " shell setprop debug.oculus.textureHeight " + dialog.customTextureHeightSelected();
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
    } else {
        switch (dialog.textureSelected()) {
        case 0:
            break;
        case 1:
            execute_true = true;
            cstring = m_adbPrefix + " shell setprop debug.oculus.textureWidth 1536";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            cstring = m_adbPrefix + " shell setprop debug.oculus.textureHeight 1690";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            break;
        case 2:
            execute_true = true;
            cstring = m_adbPrefix + " shell setprop debug.oculus.textureWidth 1440";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            cstring = m_adbPrefix + " shell setprop debug.oculus.textureHeight 1584";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            break;
        default:
            break;
        }
    }

    if (dialog.customCaptureHeightSelected().toInt(&validInt) && dialog.customCaptureWidthSelected().toInt(&validInt)) {
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.capture.width " + dialog.customCaptureWidthSelected();
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        cstring = m_adbPrefix + " shell setprop debug.oculus.capture.height " + dialog.customCaptureHeightSelected();
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
    } else {
        switch (dialog.recordingSelected()) {
        case 0:
            break;
        case 1:
            execute_true = true;
            cstring = m_adbPrefix + " shell setprop debug.oculus.capture.width 852";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            cstring = m_adbPrefix + " shell setprop debug.oculus.capture.height 480";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            break;
        case 2:
            execute_true = true;
            cstring = m_adbPrefix + " shell setprop debug.oculus.capture.width 1280";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            cstring = m_adbPrefix + " shell setprop debug.oculus.capture.height 720";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            break;
        case 3:
            execute_true = true;
            cstring = m_adbPrefix + " shell setprop debug.oculus.capture.width 1920";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            cstring = m_adbPrefix + " shell setprop debug.oculus.capture.height 1280";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            break;
        case 4:
            execute_true = true;
            cstring = m_adbPrefix + " shell setprop debug.oculus.capture.width 1024";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            cstring = m_adbPrefix + " shell setprop debug.oculus.capture.height 1024";
            command = ::getadbOutput(cstring);
            ::logfile(cstring);
            ::logfile(command);
            break;
        default:
            break;
        }
    }

    switch (dialog.proximitySelected()) {
    case 0:
        break;
    case 1:
        execute_true = true;
        cstring = m_adbPrefix + " shell am broadcast -a com.oculus.vrpowermanager.prox_close";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 2:
        execute_true = true;
        cstring = m_adbPrefix + " shell am broadcast -a com.oculus.vrpowermanager.automation_disable";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    default:
        break;
    }

    switch (dialog.guardianSelected()) {
    case 0:
        break;
    case 1:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.guardian_pause 0";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 2:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.guardian_pause 1";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    default:
        break;
    }

    switch (dialog.powerSelected()) {
    case 0:
        break;
    case 1:
        execute_true = true;
        cstring = m_adbPrefix + " shell svc power stayon false";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 2:
        execute_true = true;
        cstring = m_adbPrefix + " shell svc power stayon true";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 3:
        execute_true = true;
        cstring = m_adbPrefix + " shell svc power stayon usb";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 4:
        execute_true = true;
        cstring = m_adbPrefix + " shell svc power stayon ac";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 5:
        execute_true = true;
        cstring = m_adbPrefix + " shell svc power stayon wireless";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    default:
        break;
    }

    switch (dialog.refreshSelected()) {
    case 0:
        break;
    case 1:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.refreshrate 72";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 2:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.refreshrate 90";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 3:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.refreshrate 120";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    default:
        break;
    }

    switch (dialog.cpuSelected()) {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.cpuLevel " + QString::number(dialog.cpuSelected() - 1);
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    default:
        break;
    }

    switch (dialog.gpuSelected()) {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.gpuLevel " + QString::number(dialog.gpuSelected() - 1);
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    default:
        break;
    }

    switch (dialog.chromaticSelected()) {
    case 0:
        break;
    case 1:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.forceChroma 0";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 2:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.forceChroma 1";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    default:
        break;
    }

    switch (dialog.ratecapSelected()) {
    case 0:
        break;
    case 1:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.fullRateCapture 1";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 2:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.fullRateCapture 0";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    default:
        break;
    }

    switch (dialog.experSelected()) {
    case 0:
        break;
    case 1:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.experimentalEnabled 1";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 2:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.experimentalEnabled 0";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    default:
        break;
    }

    switch (dialog.dynamicfovSelected()) {
    case 0:
        break;
    case 1:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.dynamic 1";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 2:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.dynamic 0";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    default:
        break;
    }

    switch (dialog.fovlevelSelected()) {
    case 0:
        break;
    case 1:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.dynamic 0";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.level 0";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 2:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.dynamic 0";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.level 1";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 3:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.dynamic 0";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.level 2";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 4:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.dynamic 0";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.level 3";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    case 5:
        execute_true = true;
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.dynamic 0";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        cstring = m_adbPrefix + " shell setprop debug.oculus.foveation.level 4";
        command = ::getadbOutput(cstring);
        ::logfile(cstring);
        ::logfile(command);
        break;
    default:
        break;
    }

    if (execute_true) {
        QMessageBox msgBox(m_parent);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Values adjusted.\n\nThese changes are not persistent. Reboot headset to return values to default."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
    }
}
