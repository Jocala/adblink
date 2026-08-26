#include "apkuidmanager.h"
#include "apkdropdialog.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QFileInfo>
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
    ApkDropDialog dialog(parentWidget, QDir::homePath(),
                         tr("Get Package Name"),
                         tr("Get Package Name"),
                         tr("Drop APK file here"));
    if (dialog.exec() != QDialog::Accepted)
        return;

    QStringList filenames = dialog.selectedFiles();
    if (filenames.isEmpty())
        return;

    logfile(QStringLiteral("Package name extraction"));
    logfile(QStringLiteral("------------------------"));

    QStringList extracted;
    QStringList failed;

    for (const QString &filename : filenames) {
        QString cstring = aaptPath + " dump badging  " + '"' + filename + '"';
        QString command = getadbOutput(cstring);
        QStringList mstringlist = command.split(QRegularExpression("[\t\n\r]"), Qt::SkipEmptyParts);

        bool found = false;
        for (const QString &item : mstringlist) {
            if (item.contains("package")) {
                QRegularExpression rx("(\\')");
                QStringList query = item.split(rx);
                if (query.size() > 1) {
                    QString packagename = query.at(1);
                    logfile(packagename);
                    extracted << packagename;
                    found = true;
                    break;
                }
            }
        }
        if (!found)
            failed << filename;
    }

    if (extracted.isEmpty() && failed.isEmpty())
        return;

    if (!failed.isEmpty()) {
        for (const QString &f : failed)
            logfile(QStringLiteral("failed: ") + QFileInfo(f).fileName());
    }

    QMessageBox msgBox(parentWidget);
    msgBox.setWindowModality(Qt::WindowModal);
    if (!failed.isEmpty() && extracted.isEmpty()) {
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(tr("Failed to extract package name(s).\nSee log for details."));
    } else if (!failed.isEmpty()) {
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QString());
        msgBox.setText(tr("%1 package(s) extracted, %2 failed.\nSee log for details.")
                           .arg(extracted.size()).arg(failed.size()));
    } else {
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QString());
        if (extracted.size() == 1)
            msgBox.setText(extracted.first() + QStringLiteral("\nSee log for details."));
        else
            msgBox.setText(tr("%1 package(s) extracted.\nSee log for details.").arg(extracted.size()));
    }
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}
