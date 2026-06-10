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
    ++m_activeInstalls;
    logfile("Installing " + filename);

    QString cstring = adbPrefix + " install -r " + '"' + filename + '"';
    QString command = runLongProcess(cstring, "installing apk(s)");
    logfile(cstring);
    logfile(command);

    if (!command.contains("uccess") || command.contains("Failure")) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("%1 install failed.\nSee log.").arg(filename));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        --m_activeInstalls;
        return false;
    }
    if (--m_activeInstalls == 0) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Install complete. See log."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
    }
    return true;
}
