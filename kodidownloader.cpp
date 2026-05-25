#include "kodidownloader.h"
#include "logfile.h"
#include "getadbdata.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QEventLoop>

KodiDownloader::KodiDownloader(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
}

KodiDownloader::~KodiDownloader() = default;

QString KodiDownloader::stripVersionV(const QString &version)
{
    QString v = version;
    return v.remove('v');
}

bool KodiDownloader::isNewerVersionAvailable(const QString &installed,
                                              const QString &latest)
{
    if (installed == "Unknown" || latest == "Unknown")
        return false;

    QStringList installedParts = installed.split('.');
    QStringList latestParts = latest.split('.');

    if (installedParts.size() < 2 || latestParts.size() < 2)
        return installed != latest;

    int installedMajor = installedParts[0].toInt();
    int installedMinor = installedParts[1].toInt();
    int latestMajor = latestParts[0].toInt();
    int latestMinor = latestParts[1].toInt();

    return (installedMajor < latestMajor)
        || (installedMajor == latestMajor && installedMinor < latestMinor);
}

QString KodiDownloader::installedVersionOnDevice(const QString &adbPrefix,
                                                  const QString &deviceAddr) const
{
    QString cmd = adbPrefix + " -s " + deviceAddr
                  + " shell dumpsys package org.xbmc.kodi | grep versionName";
    QString output = getadbOutput(cmd);
    output = output.replace("versionName=", "").trimmed();
    return output;
}

QString KodiDownloader::fetchLatestVersion()
{
    QNetworkRequest req(QUrl("https://api.github.com/repos/xbmc/xbmc/releases/latest"));
    req.setHeader(QNetworkRequest::UserAgentHeader, "adblink/1.0");

    QNetworkReply *reply = m_networkManager->get(req);
    QString result = "Unknown";

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, this, [&]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            if (!doc.isNull() && doc.isObject()) {
                result = doc.object()["tag_name"].toString();
                if (result.isEmpty())
                    result = doc.object()["name"].toString();
                result = stripVersionV(result);
            } else {
                logfile("Failed to parse Kodi version JSON");
            }
        } else {
            logfile("Failed to fetch latest Kodi version: " + reply->errorString());
        }
        reply->deleteLater();
        loop.quit();
    });
    loop.exec();

    return result;
}

void KodiDownloader::startDownload(const QString &version, int archIndex,
                                    const QString &downloadDir)
{
    QString arch, filename;
    if (archIndex == 0) {
        arch = "arm/kodi-";
        filename = "kodi-" + version + "-armeabi-v7a.apk";
    } else {
        arch = "arm64-v8a/kodi-";
        filename = "kodi-" + version + "-arm64-v8a.apk";
    }

    QDir().mkpath(downloadDir);
    QString filePath = QDir(downloadDir).filePath(filename);

    QString suffix = archIndex == 0 ? "-armeabi-v7a.apk" : "-arm64-v8a.apk";
    QUrl url("https://mirrors.kodi.tv/releases/android/"
             + arch + version + suffix);

    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);

    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::WriteOnly)) {
        emit logMessage("Failed to open file for writing: " + filePath);
        emit downloadFailed("Cannot open " + filePath);
        delete file;
        return;
    }

    QNetworkReply *reply = m_networkManager->get(req);
    connect(reply, &QNetworkReply::readyRead, this, [reply, file]() {
        file->write(reply->readAll());
    });

    connect(reply, &QNetworkReply::downloadProgress, this,
            [this](qint64 received, qint64 total) {
        emit downloadProgress(received, total);
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply, file, filePath]() {
        file->close();
        reply->deleteLater();

        if (reply->error() == QNetworkReply::NoError) {
            emit logMessage("Kodi downloaded successfully to: " + filePath);
            emit downloadCompleted(filePath);
        } else {
            QString err = reply->errorString();
            emit logMessage("Kodi download failed: " + err);
            emit downloadFailed(err);
        }
        file->deleteLater();
    });
}
