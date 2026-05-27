#include "aboutmanager.h"
#include "about.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWidget>

AboutManager::AboutManager(QObject *parent)
    : QObject(parent)
{
}

void AboutManager::showAbout(QWidget *parentWidget, const QString &databaseDir,
                              const QString &versionText)
{
    QString donation;
    QFile file(databaseDir + "adblink.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (!doc.isNull()) {
            QJsonObject obj = doc.object();
            donation = obj["donation"].toString();
        }
        file.close();
    }

    Dialog2 dialog2(parentWidget, donation);
    dialog2.setWindowModality(Qt::WindowModal);
    dialog2.setaLabel(versionText);
    dialog2.exec();
}
