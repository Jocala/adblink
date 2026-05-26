// adbutils.h
#ifndef ADBUTILS_H
#define ADBUTILS_H

#include <QString>

// Get the path to the ADB binary
QString getadbpath();

// Read battery level from device via dumpsys battery
QString readBatteryLevel(const QString &adbPrefix);

// Turn off device display via keyevent 26
void displayOff(const QString &adbPrefix);

#endif // ADBUTILS_H
