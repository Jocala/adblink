#include "kodidatamanager.h"
#include "logfile.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

KodiDataManager::KodiDataManager(QObject *parent)
    : QObject(parent)
{
}

bool KodiDataManager::initializeDatabase(const QString &dbPath)
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        logfile(QStringLiteral("Error opening database: ") + dbPath + QStringLiteral(" - ") + db.lastError().text());
        return false;
    }
    return true;
}

void KodiDataManager::createTables()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        logfile(QStringLiteral("Error: Database not open"));
        return;
    }

    QSqlQuery checkQuery(db);
    bool tableExists = false;
    if (checkQuery.exec(QStringLiteral("SELECT name FROM sqlite_master WHERE type='table' AND name='device';"))
        && checkQuery.next()) {
        tableExists = true;
    } else if (checkQuery.lastError().isValid()) {
        logfile(QStringLiteral("Error checking for device table: ") + checkQuery.lastError().text());
        return;
    }

    if (!tableExists) {
        QString sql = QStringLiteral(
            "CREATE TABLE IF NOT EXISTS device ("
            "Id INTEGER PRIMARY KEY, "
            "daddr TEXT, "
            "description TEXT NOT NULL UNIQUE, "
            "pulldir TEXT, "
            "xbmcpackage TEXT, "
            "data_root TEXT, "
            "buffermode INTEGER, "
            "buffersize TEXT, "
            "bufferfactor TEXT, "
            "filepath TEXT, "
            "port TEXT, "
            "isusb INTEGER, "
            "ostype TEXT, "
            "logfilename TEXT, "
            "disableroot INTEGER, "
            "flag1 TEXT, "
            "flag2 TEXT, "
            "flag3 TEXT, "
            "flag4 TEXT, "
            "flag5 TEXT)");

        QSqlQuery query(db);
        if (!query.exec(sql)) {
            logfile(QStringLiteral("SQLite error: ") + query.lastError().text());
        } else {
            logfile(QStringLiteral("Successfully created new device table"));
        }
    }
}

DeviceRecord KodiDataManager::queryDeviceRecord(const QString &description) const
{
    DeviceRecord record;
    QSqlQuery query;

    QString sql = QStringLiteral(
        "SELECT Id, daddr, pulldir, xbmcpackage, data_root, buffermode, buffersize, "
        "bufferfactor, description, filepath, port, isusb, disableroot, flag1, flag2, ostype, flag5 "
        "FROM device WHERE description = ?");

    query.prepare(sql);
    query.addBindValue(description);
    query.exec();
    while (query.next()) {
        record.id = query.value(QStringLiteral("Id")).toInt();
        record.daddr = query.value(QStringLiteral("daddr")).toString();
        record.pulldir = query.value(QStringLiteral("pulldir")).toString();
        record.xbmcpackage = query.value(QStringLiteral("xbmcpackage")).toString();
        record.data_root = query.value(QStringLiteral("data_root")).toString();
        record.buffermode = query.value(QStringLiteral("buffermode")).toInt();
        record.buffersize = query.value(QStringLiteral("buffersize")).toString();
        record.bufferfactor = query.value(QStringLiteral("bufferfactor")).toString();
        record.description = query.value(QStringLiteral("description")).toString();
        record.filepath = query.value(QStringLiteral("filepath")).toString();
        record.port = query.value(QStringLiteral("port")).toString();
        record.isusb = query.value(QStringLiteral("isusb")).toBool();
        record.disableroot = query.value(QStringLiteral("disableroot")).toBool();
        record.scoped = query.value(QStringLiteral("flag1")).toBool();
        record.wsa = query.value(QStringLiteral("flag2")).toBool();
        record.ostype = query.value(QStringLiteral("ostype")).toString();
        record.scrcpyarg = query.value(QStringLiteral("flag5")).toString();
    }
    if (query.lastError().isValid()) {
        logfile(sql);
        logfile(QStringLiteral("SqLite error:") + query.lastError().text());
    }
    return record;
}

DeviceRecord KodiDataManager::queryDeviceByDaddr(const QString &daddr) const
{
    DeviceRecord record;
    QSqlQuery query;

    QString sql = QStringLiteral(
        "SELECT Id, daddr, pulldir, xbmcpackage, data_root, buffermode, buffersize, "
        "bufferfactor, description, filepath, port, isusb, disableroot, flag1, flag2, ostype, flag5 "
        "FROM device WHERE daddr = ?");

    query.prepare(sql);
    query.addBindValue(daddr);
    query.exec();
    while (query.next()) {
        record.id = query.value(QStringLiteral("Id")).toInt();
        record.daddr = query.value(QStringLiteral("daddr")).toString();
        record.pulldir = query.value(QStringLiteral("pulldir")).toString();
        record.xbmcpackage = query.value(QStringLiteral("xbmcpackage")).toString();
        record.data_root = query.value(QStringLiteral("data_root")).toString();
        record.buffermode = query.value(QStringLiteral("buffermode")).toInt();
        record.buffersize = query.value(QStringLiteral("buffersize")).toString();
        record.bufferfactor = query.value(QStringLiteral("bufferfactor")).toString();
        record.description = query.value(QStringLiteral("description")).toString();
        record.filepath = query.value(QStringLiteral("filepath")).toString();
        record.port = query.value(QStringLiteral("port")).toString();
        record.isusb = query.value(QStringLiteral("isusb")).toBool();
        record.disableroot = query.value(QStringLiteral("disableroot")).toBool();
        record.scoped = query.value(QStringLiteral("flag1")).toBool();
        record.wsa = query.value(QStringLiteral("flag2")).toBool();
        record.ostype = query.value(QStringLiteral("ostype")).toString();
        record.scrcpyarg = query.value(QStringLiteral("flag5")).toString();
    }
    return record;
}

bool KodiDataManager::descriptionExists(const QString &description) const
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT COUNT(*) FROM device WHERE description = ?"));
    query.addBindValue(description);
    if (query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}

void KodiDataManager::deleteRecord(const QString &description)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("DELETE FROM device WHERE description = ?"));
    query.addBindValue(description);
    query.exec();
    if (query.lastError().isValid()) {
        logfile(QStringLiteral("SqLite error:") + query.lastError().text());
    }
}

void KodiDataManager::createJsonConfig(const QString &configPath)
{
    QFile jsonFile(configPath);
    QJsonObject config;

    QDir dir(QFileInfo(configPath).absolutePath());
    dir.mkpath(QStringLiteral("."));

    struct Default { const char *key; QJsonValue value; };
    const Default defaults[] = {
        {"checkversion", true},
        {"scrcpy", true},
        {"startview", true},
        {"bypassdisconnect", false},
        {"diagnostic", false},
        {"defaultwindow", true},
        {"dropdown", QStringLiteral("0")},
        {"fmfont", 0},
        {"lgfont", 0},
        {"mdfont", 0},
        {"smfont", 0},
        {"download", QDir::homePath()},
        {"install", QDir::homePath()},
        {"backup", QDir::homePath()},
        {"localadb", QString()},
        {"protectfiles", QString()},
        {"stopapp", QStringLiteral("org.xbmc.kodi")},
        {"startapp", QStringLiteral("org.xbmc.kodi/org.xbmc.kodi.Splash")},
    };

    auto writeJson = [](QFile &file, const QJsonObject &obj) {
        if (!file.open(QIODevice::WriteOnly)) return false;
        file.write(QJsonDocument(obj).toJson());
        file.close();
        return true;
    };

    if (QFileInfo::exists(configPath)) {
        if (jsonFile.open(QIODevice::ReadOnly)) {
            if (jsonFile.size() == 0) {
                logfile(QStringLiteral("Empty JSON file: adblink.json"));
                config = QJsonObject();
                for (const auto &d : defaults)
                    config[d.key] = d.value;
            } else {
                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll(), &error);
                jsonFile.close();
                if (error.error != QJsonParseError::NoError) {
                    logfile(QStringLiteral("Invalid JSON format in adblink.json"));
                    config = QJsonObject();
                    for (const auto &d : defaults)
                        config[d.key] = d.value;
                } else {
                    config = doc.object();
                    bool needsUpdate = false;
                    QJsonObject newConfig;
                    for (const auto &d : defaults) {
                        QString key = QString::fromLatin1(d.key);
                        newConfig[key] = config.contains(key) ? config[key] : d.value;
                        if (!config.contains(key)) needsUpdate = true;
                    }
                    for (const QString &key : config.keys()) {
                        bool found = false;
                        for (const auto &d : defaults) {
                            if (QString::fromLatin1(d.key) == key) { found = true; break; }
                        }
                        if (!found) {
                            logfile(QStringLiteral("Obsolete key found: ") + key);
                            needsUpdate = true;
                        }
                    }
                    config = newConfig;
                    if (needsUpdate) {
                        QFile tmp(configPath + QStringLiteral(".tmp"));
                        if (writeJson(tmp, config)) {
                            QFile::remove(configPath);
                            tmp.rename(configPath);
                        }
                    }
                }
            }
        } else {
            logfile(QStringLiteral("Failed to read adblink.json"));
        }
    } else {
        for (const auto &d : defaults)
            config[QString::fromLatin1(d.key)] = d.value;
        writeJson(jsonFile, config);
    }

    // Load font sizes
    int lg = config[QStringLiteral("lgfont")].toInt(0);
    int md = config[QStringLiteral("mdfont")].toInt(0);
    int sm = config[QStringLiteral("smfont")].toInt(0);

    const int lgSizes[] = {14, 16, 18, 20};
    const int mdSizes[] = {12, 14, 16, 18};
    const int smSizes[] = {10, 12, 14, 16};

    lfontsize = (lg >= 0 && lg < 4) ? lgSizes[lg] : 16;
    mfontsize = (md >= 0 && md < 4) ? mdSizes[md] : 14;
    sfontsize = (sm >= 0 && sm < 4) ? smSizes[sm] : 12;
}

QString KodiDataManager::readJsonValue(const QString &configPath, const QString &key) const
{
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly))
        return QString();
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    return doc.object()[key].toString();
}

void KodiDataManager::writeJsonValue(const QString &configPath, const QString &key, const QString &value)
{
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();
    obj[key] = value;
    doc.setObject(obj);
    file.close();
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

QString KodiDataManager::readBackupPath(const QString &configPath) const
{
    return readJsonValue(configPath, QStringLiteral("backup"));
}

void KodiDataManager::writeBackupPath(const QString &configPath, const QString &dir)
{
    QDir adir(dir);
    adir.cdUp();
    writeJsonValue(configPath, QStringLiteral("backup"), adir.absolutePath());
}

QString KodiDataManager::readInstallPath(const QString &configPath) const
{
    QString install = readJsonValue(configPath, QStringLiteral("install"));
    if (install.isNull() || install.isEmpty())
        return QDir::homePath();
    return install;
}

void KodiDataManager::writeInstallPath(const QString &configPath, const QString &install)
{
    writeJsonValue(configPath, QStringLiteral("install"), install);
}

QString KodiDataManager::readDonationValue(const QString &configPath) const
{
    return readJsonValue(configPath, QStringLiteral("donation"));
}

QStringList KodiDataManager::readProtectFiles(const QString &configPath) const
{
    QString value = readJsonValue(configPath, QStringLiteral("protectfiles"));
    if (value.isEmpty())
        return QStringList();
    return value.split(',', Qt::SkipEmptyParts);
}

void KodiDataManager::writeProtectFiles(const QString &configPath, const QStringList &files)
{
    writeJsonValue(configPath, QStringLiteral("protectfiles"), files.join(','));
}
