#include <QtTest>
#include "devicerecord.h"

class TestDeviceRecord : public QObject
{
    Q_OBJECT

private slots:
    void defaultIdIsZero()
    {
        DeviceRecord rec;
        QCOMPARE(rec.id, 0);
    }

    void defaultXbmcPackage()
    {
        DeviceRecord rec;
        QCOMPARE(rec.xbmcpackage, QStringLiteral("org.xbmc.kodi"));
    }

    void defaultDataRoot()
    {
        DeviceRecord rec;
        QCOMPARE(rec.data_root, QStringLiteral("/sdcard/"));
    }

    void defaultBufferMode()
    {
        DeviceRecord rec;
        QCOMPARE(rec.buffermode, 1);
    }

    void defaultFilePath()
    {
        DeviceRecord rec;
        QCOMPARE(rec.filepath, QStringLiteral("files/.kodi"));
    }

    void defaultBooleans()
    {
        DeviceRecord rec;
        QVERIFY(!rec.isusb);
        QVERIFY(!rec.disableroot);
        QVERIFY(!rec.scoped);
        QVERIFY(!rec.wsa);
    }

    void defaultOsType()
    {
        DeviceRecord rec;
        QCOMPARE(rec.ostype, QStringLiteral("0"));
    }

    void emptyStringsByDefault()
    {
        DeviceRecord rec;
        QVERIFY(rec.daddr.isEmpty());
        QVERIFY(rec.pulldir.isEmpty());
        QVERIFY(rec.buffersize.isEmpty());
        QVERIFY(rec.bufferfactor.isEmpty());
        QVERIFY(rec.description.isEmpty());
        QVERIFY(rec.port.isEmpty());
        QVERIFY(rec.scrcpyarg.isEmpty());
    }

    void assignmentAndReadback()
    {
        DeviceRecord rec;
        rec.id = 42;
        rec.daddr = QStringLiteral("192.168.1.100");
        rec.port = QStringLiteral("5555");
        rec.description = QStringLiteral("My Device");
        rec.isusb = true;
        rec.disableroot = true;
        rec.scoped = true;
        rec.wsa = true;
        rec.ostype = QStringLiteral("2");
        rec.scrcpyarg = QStringLiteral("--max-size=1080");

        QCOMPARE(rec.id, 42);
        QCOMPARE(rec.daddr, QStringLiteral("192.168.1.100"));
        QCOMPARE(rec.port, QStringLiteral("5555"));
        QCOMPARE(rec.description, QStringLiteral("My Device"));
        QVERIFY(rec.isusb);
        QVERIFY(rec.disableroot);
        QVERIFY(rec.scoped);
        QVERIFY(rec.wsa);
        QCOMPARE(rec.ostype, QStringLiteral("2"));
        QCOMPARE(rec.scrcpyarg, QStringLiteral("--max-size=1080"));
    }
};

QTEST_APPLESS_MAIN(TestDeviceRecord)
#include "tst_devicerecord.moc"
