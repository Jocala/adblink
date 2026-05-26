#include "timermanager.h"
#include "devicerecord.h"
#include "sleepdialog.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QMessageBox>
#include <QWidget>

TimerManager::TimerManager(QObject *parent)
    : QObject(parent)
{
}

void TimerManager::pushTimers(QWidget *parentWidget,
                               const DeviceRecord &device,
                               const QString &adbPrefix)
{
    QString cstring;
    QString command;
    QString android;

    cstring = adbPrefix + " shell getprop ro.build.version.release";
    command = getadbOutput(cstring);
    android = command;

    sleepDialog dialog(parentWidget);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setFixedSize(450, 300);

    if (android.toInt() < 11) {
        cstring = adbPrefix + " shell settings get secure sleep_timeout ";
        command = getadbOutput(cstring);
        command = command.simplified();
        command.replace(" ", "");
        dialog.setcurrentsleep("Current: " + command);
    } else {
        cstring = adbPrefix + " shell settings get global stay_on_while_plugged_in ";
        command = getadbOutput(cstring);
        command = command.simplified();
        command.replace(" ", "");
        dialog.setcurrentsleep("Current: " + command);
    }

    cstring = adbPrefix + " shell settings get system screen_off_timeout ";
    command = getadbOutput(cstring);
    command = command.simplified();
    command.replace(" ", "");
    dialog.setcurrentscreen("Current: " + command);

    dialog.setdevicelabel(device.description);
    dialog.setandroidlabel(android);

    if (dialog.exec() == QDialog::Accepted) {
        QString screenval = dialog.screenValue();
        QString sleepval = dialog.sleepValue();

        if (android.toInt() < 11)
            cstring = adbPrefix + " shell settings put secure sleep_timeout " + sleepval;
        else
            cstring = adbPrefix + " shell settings put global stay_on_while_plugged_in " + sleepval;

        command = getadbOutput(cstring);

        cstring = adbPrefix + " shell settings put system screen_off_timeout " + screenval;
        command = getadbOutput(cstring);

        QString sleep1;

        if (android.toInt() < 11) {
            cstring = adbPrefix + " shell settings get secure sleep_timeout ";
            sleep1 = getadbOutput(cstring);
            sleep1 = sleep1.simplified();
            sleep1.replace(" ", "");
        } else {
            cstring = adbPrefix + " shell settings get global stay_on_while_plugged_in ";
            sleep1 = getadbOutput(cstring);
            sleep1 = sleep1.simplified();
            sleep1.replace(" ", "");
        }

        cstring = adbPrefix + " shell settings get system screen_off_timeout ";
        QString screen1 = getadbOutput(cstring);
        screen1 = screen1.simplified();
        screen1.replace(" ", "");

        QMessageBox::information(parentWidget, "", "Sleep/Screensaver values adjusted");
        logfile("Sleep/Screensaver values adjusted");
        logfile("Screensaver:" + screenval);
        logfile("Sleep:" + sleepval);
    }
}
