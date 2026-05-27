#include "installmanager.h"
#include "logfile.h"

#include <QMessageBox>
#include <QWidget>

InstallManager::InstallManager(QObject *parent)
    : QObject(parent)
{
}

bool InstallManager::installApk(QWidget *parentWidget, const QString &adbPrefix,
                                 const QString &filename,
                                 RunLongProcessCallback runLongProcess)
{
    logfile("Installing " + filename);

    QString cstring = adbPrefix + " install -r " + '"' + filename + '"';
    QString command = runLongProcess(cstring, "installing apk(s)");
    logfile(cstring);
    logfile(command);

    if (!command.contains("uccess") || command.contains("Failure")) {
        QMessageBox::critical(parentWidget, "", filename + " install failed.\nSee log.");
        return false;
    }
    return true;
}
