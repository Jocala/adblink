#include <QtTest>
#include "kodidatamanager.h"

#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QDir>

class TestJsonConfig : public QObject
{
    Q_OBJECT

private:
    KodiDataManager *mgr = nullptr;
    QTemporaryDir m_tmp;
    QString m_jsonPath;

private slots:
    void initTestCase()
    {
        QVERIFY(m_tmp.isValid());
        m_jsonPath = m_tmp.path() + "/adblink.json";
        mgr = new KodiDataManager(this);

        QString dbPath = m_tmp.path() + "/test.db";
        QVERIFY(mgr->initializeDatabase(dbPath));
        mgr->createTables();

        // createJsonConfig writes the initial file so writeJsonValue can
        // open it for read-modify-write.
        mgr->createJsonConfig(m_jsonPath);
        QVERIFY(QFile::exists(m_jsonPath));
    }

    void cleanupTestCase()
    {
        QSqlDatabase::database().close();
    }

    void writeAndReadValue()
    {
        mgr->writeJsonValue(m_jsonPath, "testkey", "testvalue");
        QCOMPARE(mgr->readJsonValue(m_jsonPath, "testkey"),
                 QStringLiteral("testvalue"));
    }

    void readMissingKeyReturnsEmpty()
    {
        QCOMPARE(mgr->readJsonValue(m_jsonPath, "noSuchKey"),
                 QStringLiteral(""));
    }

    void overwriteExistingKey()
    {
        mgr->writeJsonValue(m_jsonPath, "replaceMe", "old");
        mgr->writeJsonValue(m_jsonPath, "replaceMe", "new");
        QCOMPARE(mgr->readJsonValue(m_jsonPath, "replaceMe"),
                 QStringLiteral("new"));
    }

    void readBackupPath()
    {
        // writeBackupPath calls QDir::cdUp() on the argument before storing.
        // Just verify the round-trip produces a non-empty result.
        mgr->writeBackupPath(m_jsonPath, QDir::temp().filePath("adblink_test"));
        QString result = mgr->readBackupPath(m_jsonPath);
        QVERIFY(!result.isEmpty());
    }

    void readInstallPath()
    {
        mgr->writeInstallPath(m_jsonPath, "/tmp/install");
        QCOMPARE(mgr->readInstallPath(m_jsonPath),
                 QStringLiteral("/tmp/install"));
    }

    void readDonationValue()
    {
        mgr->writeJsonValue(m_jsonPath, "donation", "yes");
        QCOMPARE(mgr->readDonationValue(m_jsonPath),
                 QStringLiteral("yes"));
    }

    void protectFilesRoundTrip()
    {
        mgr->writeProtectFiles(m_jsonPath, {"guisettings.xml", "sources.xml", "passwords.xml"});
        QCOMPARE(mgr->readProtectFiles(m_jsonPath),
                 QStringList({"guisettings.xml", "sources.xml", "passwords.xml"}));
    }

    void protectFilesEmptyRoundTrip()
    {
        mgr->writeProtectFiles(m_jsonPath, QStringList());
        QVERIFY(mgr->readProtectFiles(m_jsonPath).isEmpty());
    }
};

QTEST_MAIN(TestJsonConfig)
#include "tst_jsonconfig.moc"
