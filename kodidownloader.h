#ifndef KODIDOWNLOADER_H
#define KODIDOWNLOADER_H

#include <QObject>
#include <QString>

class QNetworkAccessManager;

class KodiDownloader : public QObject
{
    Q_OBJECT

public:
    explicit KodiDownloader(QObject *parent = nullptr);
    ~KodiDownloader() override;

    // ADB
    QString installedVersionOnDevice(const QString &adbPrefix,
                                     const QString &deviceAddr) const;

    // Network — synchronous fetch (uses QEventLoop internally)
    QString fetchLatestVersion();

    // Network — async download (emits signals)
    void startDownload(const QString &version, int archIndex,
                       const QString &downloadDir);

    // Utilities
    static bool isNewerVersionAvailable(const QString &installed,
                                        const QString &latest);
    static QString stripVersionV(const QString &version);

signals:
    void logMessage(const QString &msg) const;
    void downloadProgress(qint64 received, qint64 total);
    void downloadCompleted(const QString &filePath);
    void downloadFailed(const QString &error);

private:
    QNetworkAccessManager *m_networkManager;
};

#endif // KODIDOWNLOADER_H
