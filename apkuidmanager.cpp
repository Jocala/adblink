#include "apkuidmanager.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QFileDialog>
#include <QMessageBox>
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

    // aaptPath has embedded quotes from mainwindow.cpp construction;
    // getadbOutput uses splitCommand which handles the quoting correctly.
    QString cstring = aaptPath + " dump badging  " + '"' + filename + '"';
    QString command = getadbOutput(cstring);

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
