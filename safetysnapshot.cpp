#include "safetysnapshot.h"

#include <QRegularExpression>

#include <algorithm>

namespace SafetySnapshot {

int parseKeepSetting(const QString &value)
{
    bool ok = false;
    const int keep = value.trimmed().toInt(&ok);
    if (!ok)
        return DefaultKeep;
    return std::clamp(keep, 0, 20);
}

QString deviceFolderName(const QString &daddr)
{
    QString name = daddr.trimmed();
    name.replace(QRegularExpression(QStringLiteral("[^A-Za-z0-9._-]")), QStringLiteral("_"));
    if (name.isEmpty() || name == QStringLiteral(".") || name == QStringLiteral(".."))
        name = QStringLiteral("device");
    return name;
}

QString deviceSnapshotRoot(const QString &configDir, const QString &daddr)
{
    QString root = configDir;
    if (!root.endsWith(QLatin1Char('/')))
        root += QLatin1Char('/');
    return root + QStringLiteral("safety/") + deviceFolderName(daddr);
}

QString snapshotFolderName(const QDateTime &when)
{
    return when.toString(QStringLiteral("yyyy-MM-dd_HHmmss"));
}

bool isSnapshotFolderName(const QString &name)
{
    static const QRegularExpression re(QStringLiteral("^\\d{4}-\\d{2}-\\d{2}_\\d{6}$"));
    return re.match(name).hasMatch();
}

static QStringList sortedSnapshots(const QStringList &names)
{
    QStringList snaps;
    for (const QString &n : names) {
        if (isSnapshotFolderName(n))
            snaps.append(n);
    }
    std::sort(snaps.begin(), snaps.end()); // name order == time order
    return snaps;
}

QStringList snapshotsToPrune(const QStringList &names, int keep)
{
    const QStringList snaps = sortedSnapshots(names);
    if (keep < 0)
        keep = 0;
    if (snaps.size() <= keep)
        return QStringList();
    return snaps.mid(0, snaps.size() - keep);
}

QString latestSnapshot(const QStringList &names)
{
    const QStringList snaps = sortedSnapshots(names);
    return snaps.isEmpty() ? QString() : snaps.last();
}

bool adbPullSucceeded(const QString &output)
{
    // Modern adb: "/sdcard/...: 1234 files pulled, 0 skipped. 5.1 MB/s ..."
    // Older adb:  "pull: building file list...\n1234 files pulled. 0 files skipped."
    // Any failure prints "adb: error: ..." or "error: ..." (device gone etc.)
    if (output.contains(QStringLiteral("error:"), Qt::CaseInsensitive))
        return false;
    static const QRegularExpression pulled(QStringLiteral("\\b[1-9]\\d* files? pulled"));
    return pulled.match(output).hasMatch();
}

} // namespace SafetySnapshot
