#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

QString getlocaladb(const QString &configDir)
{
    QString dir = QDir::cleanPath(configDir);
    if (!dir.endsWith('/'))
        dir += '/';

    QString jsonPath = dir + "adblink.json";
    if (QFileInfo::exists(jsonPath)) {
        QFile file(jsonPath);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            file.close();

            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj.contains("localadb") && !obj["localadb"].toString().isEmpty()) {
                    QString adbPath = QDir::cleanPath(obj["localadb"].toString()) + "/adb";
#ifdef Q_OS_WIN
                    adbPath += ".exe";
#endif
                    if (QFileInfo::exists(adbPath)) {
                        return adbPath;
                    }
                }
            }
        }
    }

    return QString();
}

QString getlocaladb()
{
#ifdef Q_OS_WIN
    return getlocaladb(QDir::homePath() + "/AppData/Roaming/.jocala/");
#else
    return getlocaladb(QDir::homePath() + "/.jocala/");
#endif
}
