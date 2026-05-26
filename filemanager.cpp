#include "filemanager.h"
#include "usbfiledialog.h"
#include "adbutils.h"
#include "getadbdata.h"
#include "getreturncode.h"
#include "logfile.h"

#include <QDir>
#include <QSettings>

void FileManager::openFileManager(QWidget *parent, const DeviceRecord &device,
                                   const Context &ctx)
{
    QString port;
    QString daddr;

    if (device.isusb) {
        port.clear();
        daddr = device.daddr;
    } else {
        port = device.port.isEmpty() ? "5555" : device.port;
        daddr = device.daddr + ":" + port;
    }

    bool iskodi = ctx.isKodiTab;

    m_dialog = new usbfileDialog(iskodi, ctx.fmfont, parent);
    m_dialog->setWindowModality(Qt::NonModal);

    busybox_permissions(ctx.adbPrefix);

    QString cstring = ctx.adbPrefix + " shell ls /data/local/tmp/adblink/busybox";
    if (!getreturncode(cstring)) {
        if (!ensureBusyboxInstalled(parent, ctx.adbPrefix, "Busybox not found. Install?"))
            return;
    }

    QString mcpath = resolveKodiPath(ctx.adbPrefix, device.data_root,
                                      device.xbmcpackage, false);

    m_dialog->setkodiPath(mcpath);
    m_dialog->setfmfont(ctx.fmfont);

    QString adbArg = QString("\"%1\" -s %2").arg(getadbpath(), daddr);
    m_dialog->setADB(adbArg);
    m_dialog->setData(device.description);

    QString kp = device.data_root;
    QString fmpullpath = QDir::homePath();

    if (!device.pulldir.isEmpty()) {
        QDir pulldir(device.pulldir);
        if (pulldir.exists()) {
            fmpullpath = device.pulldir;
        } else {
            logfile("Pull path: " + device.pulldir + " not found");
        }
    } else if (!ctx.downloadDir.isEmpty()) {
        QDir downloaddir(ctx.downloadDir);
        if (downloaddir.exists()) {
            fmpullpath = ctx.downloadDir;
        } else {
            logfile("Download path: " + ctx.downloadDir + " not found");
        }
    }

    if (fmpullpath == QDir::homePath() && device.pulldir.isEmpty() && ctx.downloadDir.isEmpty()) {
        logfile("Defaulting to home directory: " + QDir::homePath());
    }

    m_dialog->setPath1("/sdcard/");
    m_dialog->setPath2("/sdcard/");

    m_dialog->setuProgram(kp);
    m_dialog->setPulldir(fmpullpath);
    m_dialog->setAdbdir(ctx.apphome);

    connect(m_dialog, &QDialog::finished, this, &FileManager::handleFinished);

    QSettings settings("jocala", "adblink");
    QByteArray savedGeometry = settings.value("fmdialogGeometry").toByteArray();
    if (!savedGeometry.isEmpty())
        m_dialog->restoreGeometry(savedGeometry);

    m_dialog->show();
}

void FileManager::handleFinished()
{
    if (m_dialog) {
        QByteArray geometryData = m_dialog->saveGeometry();
        QSettings settings("jocala", "adblink");
        settings.setValue("fmdialogGeometry", geometryData);
    }
}

FileManager::FileManager(QObject *parent)
    : QObject(parent)
{
}
