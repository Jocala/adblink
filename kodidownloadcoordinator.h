#ifndef KODIDOWNLOADCOORDINATOR_H
#define KODIDOWNLOADCOORDINATOR_H

#include <QObject>
#include <QString>
#include <functional>

class KodiDownloader;
class QProgressBar;
class QLabel;
class QWidget;

class KodiDownloadCoordinator : public QObject
{
    Q_OBJECT

public:
    using InstallCallback = std::function<bool(const QString &filePath)>;
    using ShowLogCallback = std::function<void()>;
    using ServerLabelCallback = std::function<void()>;

    explicit KodiDownloadCoordinator(QObject *parent = nullptr);

    void downloadKodi(QWidget *parentWidget,
                      const QString &databaseDir,
                      KodiDownloader *downloader,
                      QProgressBar *progressBar,
                      QLabel *statusLabel,
                      QWidget *containerWidget,
                      InstallCallback installApk,
                      ShowLogCallback showLog,
                      ServerLabelCallback serverLabel);
};

#endif // KODIDOWNLOADCOORDINATOR_H
