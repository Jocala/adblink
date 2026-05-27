#include "apkuidmanager.h"
#include "logfile.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QRegularExpression>
#include <QStringList>
#include <QDir>
#include <QWidget>
#include <QApplication>

ApkUidManager::ApkUidManager(QObject *parent)
    : QObject(parent)
{
}

void ApkUidManager::getApkPackageName(QWidget *parentWidget, const QString &aaptPath)
{
    QString filename = QFileDialog::getOpenFileName(
        parentWidget, "Open APK File", QDir::homePath(),
        "APK Files (*.apk);;All Files (*)");

    if (filename.isEmpty())
        return;

    QString cstring = aaptPath + " dump badging  " + '"' + filename + '"';

    QProcess run_command;
    run_command.setProcessChannelMode(QProcess::MergedChannels);
    run_command.start(cstring);
    run_command.waitForStarted();
    while (run_command.state() != QProcess::NotRunning)
        qApp->processEvents();
    QString command = run_command.readAll();

    QStringList mstringlist = command.split(QRegularExpression("[\t\n\r]"), Qt::SkipEmptyParts);

    for (QStringList::iterator it = mstringlist.begin();
         it != mstringlist.end(); ++it) {
        QString item = *it;
        if (item.contains("package")) {
            QRegularExpression rx("(\\')");
            QStringList query = item.split(rx);
            QString packagename = query.at(1);
            logfile("package name query: " + packagename);
            QMessageBox::information(parentWidget, "", packagename);
        }
    }
}
