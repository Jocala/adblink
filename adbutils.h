// adbutils.h
#ifndef ADBUTILS_H
#define ADBUTILS_H

#include <QString>

class QWidget;

// Get the path to the ADB binary
QString getadbpath();

// Read battery level from device via dumpsys battery
QString readBatteryLevel(const QString &adbPrefix);

// Turn off device display via keyevent 26
void displayOff(const QString &adbPrefix);

// Set busybox permissions on device
void busybox_permissions(const QString &adbPrefix);

// Resolve Kodi userdata path via xbmc_env.properties or data_root derivation
QString resolveKodiPath(const QString &adbPrefix, const QString &dataRoot,
                        const QString &xbmcpackage, bool scoped);

// Install busybox on device if the user agrees
bool ensureBusyboxInstalled(QWidget *parent, const QString &adbPrefix, const QString &msg);

// Check if a package is installed on the device
bool isPackageInstalled(const QString &adbPrefix, const QString &package);

// Check if scoped storage restrictions are in effect on the device
bool isScopedStorage(const QString &adbPrefix);

#endif // ADBUTILS_H
