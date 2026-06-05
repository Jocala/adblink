#include "versioncheckmanager.h"

#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStackedWidget>
#include <QMenu>
#include <QAction>
#include <QWidget>

VersionCheckManager::VersionCheckManager(QObject *parent)
    : QObject(parent)
{
}

void VersionCheckManager::checkVersion(QWidget *parentWidget,
                                        const QString &databaseDir,
                                        const QString &versionUrl,
                                        QStackedWidget *stackedWidget,
                                        QMenu *kodiMenu,
                                        QAction *infoArchAction,
                                        QNetworkAccessManager *networkManager,
                                        VersionCheckCallback onCompleted,
                                        int &currentStack)
{
    QJsonObject obj;
    QFile file(databaseDir + "adblink.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        obj = doc.object();
        file.close();
    }
    bool checkversion = obj["checkversion"].toBool();
    bool startview = obj["startview"].toBool();

    if (startview) {
        stackedWidget->setCurrentIndex(0);
        currentStack = 0;
        kodiMenu->menuAction()->setVisible(true);
        infoArchAction->setEnabled(true);
        infoArchAction->setVisible(true);
    } else {
        stackedWidget->setCurrentIndex(1);
        currentStack = 1;
        kodiMenu->menuAction()->setVisible(false);
        infoArchAction->setEnabled(false);
        infoArchAction->setVisible(false);
    }

    if (checkversion) {
        if (!QUrl(versionUrl).isValid()) {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Invalid URL for version check"));
            msgBox.setStandardButtons(QMessageBox::Cancel);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            return;
        }
        QNetworkRequest request;
        request.setUrl(QUrl(versionUrl));
        QNetworkReply *reply = networkManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [onCompleted]() {
            onCompleted();
        });
    }
}
