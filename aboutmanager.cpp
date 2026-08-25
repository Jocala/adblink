#include "aboutmanager.h"
#include "about.h"

#include <QWidget>

AboutManager::AboutManager(QObject *parent)
    : QObject(parent)
{
}

void AboutManager::showAbout(QWidget *parentWidget, const QString &databaseDir,
                              const QString &versionText, const QString &donation)
{
    Q_UNUSED(databaseDir);
    Dialog2 dialog2(parentWidget, donation);
    dialog2.setWindowModality(Qt::WindowModal);
    dialog2.setaLabel(versionText);
    dialog2.exec();
}
