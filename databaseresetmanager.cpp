#include "databaseresetmanager.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QCoreApplication>
#include <QProcess>
#include <QWidget>

DatabaseResetManager::DatabaseResetManager(QObject *parent)
    : QObject(parent)
{
}

void DatabaseResetManager::resetDatabase(QWidget *parentWidget, const QString &databaseDir)
{
    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::PlainText);
    msgBox.setText("Initialize adblink?\nWARNING: This action will delete all device records and settings, then close and restart adblink. Are you sure you want to proceed?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setIcon(QMessageBox::Critical);

    QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());

    if (reply == QMessageBox::Yes) {
        QStringList connections = QSqlDatabase::connectionNames();
        for (const QString &conn : connections) {
            QSqlDatabase db = QSqlDatabase::database(conn, false);
            if (db.isOpen()) {
                QSqlQuery query(db);
                query.clear();
                if (db.transaction())
                    db.commit();
                db.close();
            }
            QSqlDatabase::removeDatabase(conn);
        }

        QDir dir(databaseDir);
        if (dir.exists() && !dir.removeRecursively()) {
            /* #ifdef Q_OS_WIN
               QString command = QString("cmd.exe /C rmdir /S /Q \"%1\"").arg(databaseDir.replace("/", "\\"));
               QProcess::startDetached(command, QStringList());
            #endif */
        }

        QCoreApplication::quit();
        QString program = QCoreApplication::applicationFilePath();
        QStringList arguments = QCoreApplication::arguments();
        QProcess::startDetached(program, arguments);
    }
}
