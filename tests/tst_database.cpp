#include <QtTest>
#include "kodidatamanager.h"
#include "devicerecord.h"

#include <QTemporaryDir>
#include <QSqlDatabase>
#include <QSqlQuery>

class TestDatabase : public QObject
{
    Q_OBJECT

private:
    KodiDataManager *mgr = nullptr;
    QTemporaryDir m_tmp;

private slots:
    void initTestCase()
    {
        QVERIFY(m_tmp.isValid());
        mgr = new KodiDataManager(this);
        QString dbPath = m_tmp.path() + "/test.db";
        QVERIFY(mgr->initializeDatabase(dbPath));
        mgr->createTables();
    }

    void cleanupTestCase()
    {
        QSqlDatabase::database().close();
    }

    void insertAndQueryByDescription()
    {
        QSqlQuery q;
        QVERIFY(q.exec(QStringLiteral(
            "INSERT INTO device (description, daddr, port, isusb, ostype, "
            "data_root, xbmcpackage, pulldir, disableroot, filepath, flag5) "
            "VALUES ('TestDevice', '192.168.1.100', '5555', 0, '0', "
            "'/sdcard/', 'org.test.app', '/pull', 0, '/files/.test', '')")));
        QSqlDatabase::database().commit();

        DeviceRecord rec = mgr->queryDeviceRecord("TestDevice");
        QCOMPARE(rec.description, QStringLiteral("TestDevice"));
        QCOMPARE(rec.daddr, QStringLiteral("192.168.1.100"));
        QCOMPARE(rec.port, QStringLiteral("5555"));
        QCOMPARE(rec.isusb, false);
        QCOMPARE(rec.xbmcpackage, QStringLiteral("org.test.app"));
        QCOMPARE(rec.data_root, QStringLiteral("/sdcard/"));
        QCOMPARE(rec.filepath, QStringLiteral("/files/.test"));
    }

    void queryByDaddr()
    {
        QSqlQuery q;
        QVERIFY(q.exec(QStringLiteral(
            "INSERT INTO device (description, daddr, port, isusb, ostype, "
            "data_root, xbmcpackage, pulldir, disableroot, filepath, flag5) "
            "VALUES ('AddrDevice', '10.0.0.1', '5555', 0, '1', "
            "'/data/', 'org.addr.app', '/pulladdr', 0, '/files/.addr', '--max-size=1080')")));
        QSqlDatabase::database().commit();

        DeviceRecord rec = mgr->queryDeviceByDaddr("10.0.0.1");
        QCOMPARE(rec.description, QStringLiteral("AddrDevice"));
        QCOMPARE(rec.ostype, QStringLiteral("1"));
        QCOMPARE(rec.scrcpyarg, QStringLiteral("--max-size=1080"));
    }

    void descriptionExists()
    {
        QSqlQuery q;
        QVERIFY(q.exec(QStringLiteral(
            "INSERT INTO device (description, daddr, port, isusb, ostype, "
            "data_root, xbmcpackage, pulldir, disableroot, filepath, flag5) "
            "VALUES ('ExistTest', '10.0.0.2', '5555', 0, '0', "
            "'/sdcard/', 'org.xbmc.kodi', '', 0, '/files/.kodi', '')")));
        QSqlDatabase::database().commit();

        QVERIFY(mgr->descriptionExists("ExistTest"));
        QVERIFY(!mgr->descriptionExists("NoSuchDevice"));
    }

    void deleteRecord()
    {
        QSqlQuery q;
        QVERIFY(q.exec(QStringLiteral(
            "INSERT INTO device (description, daddr, port, isusb, ostype, "
            "data_root, xbmcpackage, pulldir, disableroot, filepath, flag5) "
            "VALUES ('DeleteMe', '10.0.0.3', '5555', 0, '0', "
            "'/sdcard/', 'org.xbmc.kodi', '', 0, '/files/.kodi', '')")));
        QSqlDatabase::database().commit();

        QVERIFY(mgr->descriptionExists("DeleteMe"));
        mgr->deleteRecord("DeleteMe");
        QVERIFY(!mgr->descriptionExists("DeleteMe"));
    }

    void queryNonexistentReturnsEmpty()
    {
        DeviceRecord rec = mgr->queryDeviceRecord("NoSuchRecord");
        QVERIFY(rec.description.isEmpty());
        QVERIFY(rec.daddr.isEmpty());
        QCOMPARE(rec.id, 0);
    }
};

QTEST_MAIN(TestDatabase)
#include "tst_database.moc"
