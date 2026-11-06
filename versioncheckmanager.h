#ifndef VERSIONCHECKMANAGER_H
#define VERSIONCHECKMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

class QStackedWidget;
class QMenu;
class QAction;
class QNetworkAccessManager;
class QNetworkReply;

class VersionCheckManager : public QObject
{
    Q_OBJECT

public:
    // The reply is passed through explicitly: MainWindow's handler used to
    // call sender(), which is null because this manager, not MainWindow, is
    // the connection's receiver. That silently disabled the startup check
    // from 8.0 through 8.4.
    using VersionCheckCallback = std::function<void(QNetworkReply *)>;

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
