#include "aboutmanager.h"
#include "about.h"

#include <QWidget>

AboutManager::AboutManager(QObject *parent)
    : QObject(parent)
{
}

void AboutManager::showAbout(QWidget *parentWidget, const QString &databaseDir,
                              const QString &versionText)
{
    Dialog2 dialog2(parentWidget);
    dialog2.setWindowModality(Qt::WindowModal);
    dialog2.setaLabel(versionText);
    dialog2.exec();
}
