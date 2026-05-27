#include "sendtextmanager.h"
#include "devicerecord.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QInputDialog>
#include <QLineEdit>
#include <QWidget>

SendTextManager::SendTextManager(QObject *parent)
    : QObject(parent)
{
}

void SendTextManager::sendText(QWidget *parentWidget, const DeviceRecord & /*device*/,
                                const QString &adbPrefix)
{
    bool ok;
    QString text = QInputDialog::getText(parentWidget, tr("Text to Device"),
                                          tr("Send text:"), QLineEdit::Normal,
                                          QString(), &ok);
    if (ok && !text.isEmpty()) {
        text.replace(" ", "%s");
        QString cstring = adbPrefix + " shell input text " + text;
        QString command = getadbOutput(cstring);
        logfile(cstring);
        logfile(command);
    }
}
