#ifndef SAFETYSNAPSHOT_H
#define SAFETYSNAPSHOT_H

#include <QDateTime>
#include <QString>
#include <QStringList>

// Safety snapshots: before Restore wipes Kodi's folder on a device, adbLink
// pulls the current contents to <config dir>/safety/<device>/<timestamp>/ so
// a failed or unwanted restore can always be undone (Kodi > Undo last restore).

namespace SafetySnapshot {

// Default number of snapshots kept per device. 0 disables snapshots.
constexpr int DefaultKeep = 3;

// Parse the "safetysnapshots" JSON setting; empty/invalid -> DefaultKeep,
// clamped to 0..20.
int parseKeepSetting(const QString &value);

// Folder-safe name for a device address, e.g. "192.168.1.20:5555" ->
// "192.168.1.20_5555".
QString deviceFolderName(const QString &daddr);

// <configDir>/safety/<device>
QString deviceSnapshotRoot(const QString &configDir, const QString &daddr);

// Timestamped folder name, e.g. "2026-10-09_191500". Sorts chronologically.
QString snapshotFolderName(const QDateTime &when);

// True if `name` looks like a folder created by snapshotFolderName().
bool isSnapshotFolderName(const QString &name);

// Given the folder names in a device's snapshot root, return those to delete
// so that only the newest `keep` remain (ignores unrelated names).
QStringList snapshotsToPrune(const QStringList &names, int keep);

// Newest snapshot folder name in `names`, or empty.
QString latestSnapshot(const QStringList &names);

// True if adb pull output reports a completed pull with no errors.
bool adbPullSucceeded(const QString &output);

} // namespace SafetySnapshot

#endif // SAFETYSNAPSHOT_H
