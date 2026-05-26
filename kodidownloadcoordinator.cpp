#include "kodidownloadcoordinator.h"
#include "kodidownloader.h"
#include "kodiarchdialog.h"
#include "logfile.h"

#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QProgressBar>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QAbstractButton>

KodiDownloadCoordinator::KodiDownloadCoordinator(QObject *parent)
    : QObject(parent)
{
}

void KodiDownloadCoordinator::downloadKodi(QWidget *parentWidget,
                                            const QString &databaseDir,
                                            KodiDownloader *downloader,
                                            QProgressBar *progressBar,
                                            QLabel *statusLabel,
                                            QWidget *containerWidget,
                                            InstallCallback installApk,
                                            ShowLogCallback showLog,
                                            ServerLabelCallback serverLabel)
{
    QJsonObject obj;
    QFile jsonFile(databaseDir + "adblink.json");
    if (jsonFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
        obj = doc.object();
        jsonFile.close();
    }

    QString downloadDir = obj["install"].toString();
    if (downloadDir.isEmpty())
        downloadDir = QDir::homePath();

    QString kodiVersion = downloader->fetchLatestVersion();

    KodiArchDialog archDialog(kodiVersion, parentWidget);
    if (archDialog.exec() != QDialog::Accepted)
        return;

    int arch = archDialog.selectedArch();
    if (arch == 2) {
        QDesktopServices::openUrl(QUrl("https://kodi.tv/download/android"));
        return;
    }

    if (kodiVersion == "Unknown") {
        QMessageBox::critical(parentWidget, "Error",
            "Cannot download: Kodi version unknown. See log");
        logfile("Download aborted: Unknown Kodi version");
        return;
    }

    disconnect(downloader, &KodiDownloader::downloadCompleted, nullptr, nullptr);
    disconnect(downloader, &KodiDownloader::downloadFailed, nullptr, nullptr);
    disconnect(downloader, &KodiDownloader::downloadProgress, nullptr, nullptr);

    progressBar->setHidden(false);
    progressBar->setValue(0);
    statusLabel->setText("Downloading Kodi...");
    containerWidget->setHidden(true);

    auto hideProgress = [progressBar, containerWidget, statusLabel, serverLabel]() {
        progressBar->setHidden(true);
        progressBar->setValue(0);
        containerWidget->setHidden(false);
        statusLabel->setText("");
        serverLabel();
    };

    connect(downloader, &KodiDownloader::downloadProgress,
            this, [progressBar](qint64 received, qint64 total) {
        if (total > 0)
            progressBar->setValue(static_cast<int>(received * 100 / total));
    });

    connect(downloader, &KodiDownloader::downloadCompleted,
            this, [parentWidget, hideProgress, installApk, showLog](const QString &filePath) {
        hideProgress();
        logfile("The Kodi APK file has been downloaded successfully to:\n" + filePath);

        QMessageBox msgBox(parentWidget);
        msgBox.setWindowTitle("Download Success");
        msgBox.setText("Kodi downloaded. See log for details");
        QAbstractButton *installBtn = msgBox.addButton("Install", QMessageBox::ActionRole);
        QAbstractButton *logBtn = msgBox.addButton("Logfile", QMessageBox::ActionRole);
        msgBox.addButton(QMessageBox::Ok);
        msgBox.exec();

        if (msgBox.clickedButton() == installBtn) {
            installApk(filePath);
        } else if (msgBox.clickedButton() == logBtn) {
            showLog();
        }
    });

    connect(downloader, &KodiDownloader::downloadFailed,
            this, [parentWidget, hideProgress](const QString &error) {
        hideProgress();
        logfile("Kodi download failed: " + error);
        QMessageBox::critical(parentWidget, "Download Failed",
            "Failed to download Kodi. See log");
    });

    downloader->startDownload(kodiVersion, arch, downloadDir);
}
