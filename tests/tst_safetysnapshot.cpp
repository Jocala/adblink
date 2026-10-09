#include <QtTest>
#include "safetysnapshot.h"

using namespace SafetySnapshot;

class TestSafetySnapshot : public QObject
{
    Q_OBJECT

private slots:
    void keepSetting()
    {
        QCOMPARE(parseKeepSetting(QString()), DefaultKeep);
        QCOMPARE(parseKeepSetting("abc"), DefaultKeep);
        QCOMPARE(parseKeepSetting("0"), 0);      // disabled
        QCOMPARE(parseKeepSetting(" 5 "), 5);
        QCOMPARE(parseKeepSetting("-2"), 0);
        QCOMPARE(parseKeepSetting("999"), 20);
    }

    void deviceFolderNames()
    {
        QCOMPARE(deviceFolderName("192.168.1.20:5555"), QStringLiteral("192.168.1.20_5555"));
        QCOMPARE(deviceFolderName("G070VM1234567890"), QStringLiteral("G070VM1234567890"));
        QCOMPARE(deviceFolderName("../../etc"), QStringLiteral(".._.._etc"));
        QCOMPARE(deviceFolderName(".."), QStringLiteral("device"));
        QCOMPARE(deviceFolderName(""), QStringLiteral("device"));
    }

    void snapshotRoot()
    {
        QCOMPARE(deviceSnapshotRoot("/home/a/.jocala", "10.0.0.5"),
                 QStringLiteral("/home/a/.jocala/safety/10.0.0.5"));
        QCOMPARE(deviceSnapshotRoot("/home/a/.jocala/", "10.0.0.5"),
                 QStringLiteral("/home/a/.jocala/safety/10.0.0.5"));
    }

    void folderNameRoundTrip()
    {
        const QDateTime t(QDate(2026, 10, 9), QTime(19, 15, 7));
        const QString name = snapshotFolderName(t);
        QCOMPARE(name, QStringLiteral("2026-10-09_191507"));
        QVERIFY(isSnapshotFolderName(name));
        QVERIFY(!isSnapshotFolderName("notes"));
        QVERIFY(!isSnapshotFolderName("2026-10-09"));
    }

    void pruneKeepsNewest()
    {
        const QStringList names{ "2026-10-09_191507", "2026-01-02_080000", "notes",
                                 "2026-05-05_120000", "2025-12-31_235959" };
        QCOMPARE(snapshotsToPrune(names, 2),
                 (QStringList{ "2025-12-31_235959", "2026-01-02_080000" }));
        QVERIFY(snapshotsToPrune(names, 4).isEmpty());
        QCOMPARE(snapshotsToPrune(names, 0).size(), 4);  // never touches "notes"
    }

    void latest()
    {
        QCOMPARE(latestSnapshot({ "2026-01-02_080000", "2026-10-09_191507", "zzz" }),
                 QStringLiteral("2026-10-09_191507"));
        QVERIFY(latestSnapshot({ "zzz" }).isEmpty());
    }

    void pullSucceeded()
    {
        QVERIFY(adbPullSucceeded("/sdcard/Android/data/org.xbmc.kodi/files/.kodi/.: 1534 files pulled, 0 skipped. 12.3 MB/s (412345678 bytes in 31.9s)"));
        QVERIFY(adbPullSucceeded("pull: building file list...\n1 file pulled. 0 files skipped."));
    }

    void pullFailed()
    {
        QVERIFY(!adbPullSucceeded(QString()));
        QVERIFY(!adbPullSucceeded("adb: error: failed to stat remote object '/sdcard/x': No such file or directory"));
        QVERIFY(!adbPullSucceeded("error: device '192.168.1.20:5555' not found"));
        QVERIFY(!adbPullSucceeded("0 files pulled, 0 skipped."));
        // Pulled some files then the connection dropped
        QVERIFY(!adbPullSucceeded("[ 42%] /sdcard/...\nadb: error: failed to read copy response\n"));
    }
};

QTEST_APPLESS_MAIN(TestSafetySnapshot)
#include "tst_safetysnapshot.moc"
