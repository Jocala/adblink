#include "datamovemanager.h"
#include "datadialog.h"
#include "devicerecord.h"
#include "getadbdata.h"
#include "getreturncode.h"
#include "logfile.h"

#include <QMessageBox>

void DataMoveManager::moveKodiData(QWidget *parent, const DeviceRecord &device,
                                    const QString &adbPrefix, bool scoped,
                                    RunLongProcessCallback runLongProcess)
{
    QString cstring = adbPrefix + " shell ps | grep " + device.xbmcpackage;
    QString command = getadbOutput(cstring);

    QString destination;
    QString source;
    QString kbase;
    QString n_data_root;
    int choice;

    if (command.contains(device.xbmcpackage))
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(parent, "Stop Kodi", "Cannot move data while Kodi is running.\n Stop " + device.xbmcpackage + " on device?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            QString cstring = adbPrefix + " shell am force-stop " + device.xbmcpackage;
            QString command = getadbOutput(cstring);
            logfile(command);
        }
        else
        {
            logfile(device.xbmcpackage + " running. Move data failed");
            return;
        }
    }

    cstring = adbPrefix + " shell /data/local/tmp/adblink/busybox find /storage -type d -maxdepth 1";
    QString s = getadbOutput(cstring);

    QStringList list = s.split('\n');

    for (int i = 0; i < list.size(); i++)
    {
        list[i].remove('\r');
        list[i].remove('\n');

        if (list[i] == "Android" ||
            list[i] == "Permission denied" ||
            list[i] == "/storage/emulated" ||
            list[i] == "/storage" ||
            list[i] == "/storage/self" ||
            list[i].isNull())
        {
            list.removeAt(i);
            i--;
        }
    }

    if (list.size() <= 0)
    {
        QMessageBox::critical(parent, "", "No external devices found");
        return;
    }

    dataDialog dialog(parent);
    dialog.setadb_data(list);
    dialog.setWindowModality(Qt::WindowModal);

    if (dialog.exec() == QDialog::Accepted)
    {
        n_data_root = dialog.externalLocation();
        choice = dialog.returnval2();
    }
    else
        return;

    if (!n_data_root.startsWith("/"))
        n_data_root.prepend("/");

    if (!n_data_root.endsWith("/"))
        n_data_root.append("/");

    if (choice == 1)  // sdcard to external
    {
        if (scoped)
        {
            kbase = "/sdcard/kodi_data/";
            source = kbase + device.xbmcpackage;
            destination = n_data_root + "kodi_data/" + device.xbmcpackage;
        }
        else
        {
            source = "/sdcard/Android/data/" + device.xbmcpackage;
            destination = n_data_root + "Android/data/" + device.xbmcpackage;
            kbase = source;
        }
    }

    if (choice == 2)  // external to sdcard
    {
        if (scoped)
        {
            destination = "/sdcard/kodi_data/" + device.xbmcpackage;
            source = n_data_root + "kodi_data/" + device.xbmcpackage;
            kbase = n_data_root + "kodi_data/";
        }
        else
        {
            destination = "/sdcard/Android/data/" + device.xbmcpackage;
            source = n_data_root + "Android/data/" + device.xbmcpackage;
            kbase = source;
        }
    }

    cstring = adbPrefix + " shell ls " + source + "/files/.kodi";

    if (!getreturncode(cstring))
    {
        QMessageBox::critical(parent, "", "Kodi's files not found at " + source);
        logfile("Data move: files not found at " + source);
        return;
    }

    cstring = adbPrefix + " shell ls " + destination + "/files/.kodi";

    if (getreturncode(cstring))
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(parent, "", "Kodi data already exists. Overwrite?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
        {
            return;
        }
        else
        {
            cstring = adbPrefix + " shell rm -r " + destination;
            logfile("Erasing: " + cstring);
            command = runLongProcess(cstring, "Preparing target");
            logfile(command);
        }
    }

    cstring = adbPrefix + " shell mkdir -p " + destination + "/files";
    getreturncode(cstring);

    cstring = adbPrefix + " shell cp -r " + source + "/files/.kodi " + destination + "/files";
    logfile("Kodi file move:" + cstring);
    command = runLongProcess(cstring, "Copying data to " + destination);

    cstring = adbPrefix + " shell test -e " + destination + "/files/.kodi";

    if (!getreturncode(cstring))
    {
        QMessageBox::critical(parent, "", "File copy failed. See log.");
        return;
    }

    cstring = adbPrefix + " shell test -e /sdcard/xbmc_env.properties";
    if (getreturncode(cstring))
    {
        getreturncode(adbPrefix + " shell rm /sdcard/xbmc_env.properties");
    }

    cstring = adbPrefix + " shell echo xbmc.data=" + destination + "/files" + " > /sdcard/xbmc_env.properties";

    if (!cstring.contains("/sdcard/Android/data/org.xbmc.kodi"))
    {
        if (!getreturncode(cstring))
            logfile("ERROR: " + command);
    }

    QMessageBox::StandardButton reply2;
    reply2 = QMessageBox::question(parent, "", "Erase " + kbase + "?",
                                   QMessageBox::Yes | QMessageBox::No);
    if (reply2 == QMessageBox::No)
    {
        QMessageBox::information(parent, "", "Data copy complete");
        return;
    }

    cstring = adbPrefix + " shell rm -r " + kbase;
    command = runLongProcess(cstring, "Erasing " + kbase);
    logfile("Erasing: " + cstring);

    QMessageBox::information(parent, "", "Data move complete");
}

DataMoveManager::DataMoveManager(QObject *parent)
    : QObject(parent)
{
}
