#ifndef KODIENV_H
#define KODIENV_H

#include <QString>

// Parse the contents of /sdcard/xbmc_env.properties and return the Kodi data
// root it points to, e.g. "/sdcard/kodi_data/org.xbmc.kodi" (without the
// trailing "/files"). Kodi itself always honours this file, so adbLink must
// use the same location for backup, restore and editing.
//
// Returns an empty string if there is no usable "xbmc.data=" entry: the path
// must be absolute, contain only [A-Za-z0-9._/-] (no ".."), end in
// "/<package>/files", and sit under "kodi_data" or "Android/data".
QString parseXbmcEnvDataRoot(const QString &envContent, const QString &package);

#endif // KODIENV_H
