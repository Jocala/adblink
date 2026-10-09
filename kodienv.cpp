#include "kodienv.h"

#include <QRegularExpression>
#include <QStringList>

QString parseXbmcEnvDataRoot(const QString &envContent, const QString &package)
{
    if (package.isEmpty())
        return QString();

    const QString envPrefix = QStringLiteral("xbmc.data=");
    const QStringList lines = envContent.split(QRegularExpression(QStringLiteral("[\r\n]+")),
                                               Qt::SkipEmptyParts);
    for (const QString &rawLine : lines) {
        const QString line = rawLine.trimmed();
        if (!line.startsWith(envPrefix))
            continue;

        QString path = line.mid(envPrefix.length()).trimmed();
        while (path.length() > 1 && path.endsWith(QLatin1Char('/')))
            path.chop(1);

        // The path ends up in remote shell commands (including rm -r), and the
        // file can be written by other apps, so only allow plain path chars.
        static const QRegularExpression safePath(QStringLiteral("^/[A-Za-z0-9._/-]+$"));
        if (!safePath.match(path).hasMatch() || path.contains(QStringLiteral("..")))
            return QString();

        if (!path.endsWith(QStringLiteral("/files")))
            return QString();
        path.chop(6); // remove "/files"

        const QString suffix = QLatin1Char('/') + package;
        if (!path.endsWith(suffix))
            return QString();

        // kodi_data on any volume (Data Move writes /storage/<id>/kodi_data/...)
        // or the legacy Android/data layout.
        const QString structure = path.left(path.length() - suffix.length());
        if (!structure.endsWith(QStringLiteral("/kodi_data"))
            && !structure.endsWith(QStringLiteral("/Android/data")))
            return QString();
        return path;
    }
    return QString();
}
