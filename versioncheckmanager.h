#ifndef VERSIONCHECKMANAGER_H
#define VERSIONCHECKMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

class QStackedWidget;
class QMenu;
class QAction;
class QNetworkAccessManager;

class VersionCheckManager : public QObject
{
    Q_OBJECT

public:
    using VersionCheckCallback = std::function<void()>;

    explicit VersionCheckManager(QObject *parent = nullptr);

    void checkVersion(QWidget *parentWidget,
                      const QString &databaseDir,
                      const QString &versionUrl,
                      QStackedWidget *stackedWidget,
                      QMenu *kodiMenu,
                      QAction *infoArchAction,
                      QNetworkAccessManager *networkManager,
                      VersionCheckCallback onCompleted,
                      int &currentStack);
};

#endif // VERSIONCHECKMANAGER_H
