#include "kodilogmanager.h"
#include "devicerecord.h"
#include "adbutils.h"
#include "getadbdata.h"
#include "logfile.h"
#include "logdialog.h"

#include <QMessageBox>
#include <QWidget>

KodiLogManager::KodiLogManager(QObject *parent)
    : QObject(parent)
{
}

void KodiLogManager::viewKodiLog(QWidget *parentWidget,
                                  const DeviceRecord &device,
                                  const QString &adbPrefix,
                                  const QString &busyPath)
{
    QString mcpath;

    mcpath = resolveKodiPath(adbPrefix, device.data_root, device.xbmcpackage,
                             isScopedStorage(adbPrefix));

    QString xpath = mcpath + "/temp/";

    QString cstring = adbPrefix + " shell " + busyPath + "busybox find " + xpath + " -maxdepth 1 -name kodi.log ";
    QString command = getadbOutput(cstring);

    if (command.isEmpty() || command.contains("No such file or directory")) {
        QMessageBox::critical(parentWidget, "", "Kodi log not found");
        logfile(command);
        logfile("Kodi log not found!");
        return;
    }

    DeviceRecord dev = device;
    dev.filepath = xpath;

    logfile("opening kodi log");
    logDialog logdialog;
    logdialog.loadRemoteLog(adbPrefix, dev.filepath);
    logdialog.setModal(true);
    logdialog.exec();
}
