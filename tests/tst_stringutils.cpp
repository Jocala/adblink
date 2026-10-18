#include <QtTest>
#include "stringutils.h"

class TestStringUtils : public QObject
{
    Q_OBJECT

private slots:
    // --- stripString ---
    void stripSpaces()
    {
        QCOMPARE(stripString("  a b  c  "), QStringLiteral("abc"));
    }

    void stripTabs()
    {
        QCOMPARE(stripString("\ta\tb\tc\t"), QStringLiteral("abc"));
    }

    void stripNewlines()
    {
        QCOMPARE(stripString("\na\nb\nc\n"), QStringLiteral("abc"));
    }

    void stripMixed()
    {
        QCOMPARE(stripString("  hello\tworld\n  "), QStringLiteral("helloworld"));
    }

    void stripAlreadyClean()
    {
        QCOMPARE(stripString("abc"), QStringLiteral("abc"));
    }

    void stripEmpty()
    {
        QCOMPARE(stripString(""), QStringLiteral(""));
    }

    // --- checkslash ---
    void checkslashBare()
    {
        QCOMPARE(checkslash("foo"), QStringLiteral("/foo/"));
    }

    void checkslashLeadingOnly()
    {
        QCOMPARE(checkslash("/bar"), QStringLiteral("/bar/"));
    }

    void checkslashTrailingOnly()
    {
        QCOMPARE(checkslash("baz/"), QStringLiteral("/baz/"));
    }

    void checkslashBoth()
    {
        QCOMPARE(checkslash("/qux/"), QStringLiteral("/qux/"));
    }

    void checkslashNested()
    {
        QCOMPARE(checkslash("a/b/c"), QStringLiteral("/a/b/c/"));
    }

    void checkslashEmpty()
    {
        // empty gets one "/" prepended, then endsWith check sees it already
        QCOMPARE(checkslash(""), QStringLiteral("/"));
    }

    // --- validateIPAddress ---
    void validateIP_data()
    {
        QTest::addColumn<QString>("input");
        QTest::addColumn<bool>("expected");

        QTest::newRow("localhost")   << "127.0.0.1"   << true;
        QTest::newRow("private")     << "192.168.1.1" << true;
        QTest::newRow("public")      << "8.8.8.8"     << true;
        QTest::newRow("all_zeros")   << "0.0.0.0"     << true;
        QTest::newRow("broadcast")   << "255.255.255.255" << true;
        QTest::newRow("octet_overflow_ip") << "192.168.1.256" << true; // matches hostname alt
        QTest::newRow("five_octets") << "1.2.3.4.5" << true;          // matches hostname alt
        QTest::newRow("empty")       << ""            << false;
        QTest::newRow("hostname")    << "host.local"  << true;
        QTest::newRow("no_dots")     << "justtext"    << false;
        QTest::newRow("spaces_before") << "  192.168.1.1" << true;
        QTest::newRow("with_spaces") << "not valid"   << false;
    }

    void validateIP()
    {
        QFETCH(QString, input);
        QFETCH(bool, expected);
        QCOMPARE(validateIPAddress(input), expected);
    }

    // --- usbStatus ---
    void usbStatusDeviceConnected()
    {
        QHash<QString, QString> cache;
        cache["abc123"] = "device";
        QCOMPARE(usbStatus(cache, "abc123"), QStringLiteral("Connected"));
    }

    void usbStatusOffline()
    {
        QHash<QString, QString> cache;
        cache["abc123"] = "offline";
        QCOMPARE(usbStatus(cache, "abc123"), QStringLiteral("Offline"));
    }

    void usbStatusUnauthorized()
    {
        QHash<QString, QString> cache;
        cache["abc123"] = "unauthorized";
        QCOMPARE(usbStatus(cache, "abc123"), QStringLiteral("Unauthorized"));
    }

    void usbStatusMissing()
    {
        QHash<QString, QString> cache;
        QCOMPARE(usbStatus(cache, "nonexistent"), QStringLiteral("Disconnected"));
    }

    void usbStatusEmptyCache()
    {
        QHash<QString, QString> cache;
        QCOMPARE(usbStatus(cache, "abc123"), QStringLiteral("Disconnected"));
    }

    void usbStatusUnknownStatus()
    {
        QHash<QString, QString> cache;
        cache["abc123"] = "someweirdstatus";
        QCOMPARE(usbStatus(cache, "abc123"), QStringLiteral("Someweirdstatus"));
    }

    void usbStatusRecovery()
    {
        QHash<QString, QString> cache;
        cache["abc123"] = "recovery";
        QCOMPARE(usbStatus(cache, "abc123"), QStringLiteral("Recovery"));
    }

    void usbStatusRescue()
    {
        QHash<QString, QString> cache;
        cache["abc123"] = "rescue";
        QCOMPARE(usbStatus(cache, "abc123"), QStringLiteral("Rescue"));
    }

    // --- isConnectedStatus ---
    void isConnectedStatusValues_data()
    {
        QTest::addColumn<QString>("status");
        QTest::addColumn<bool>("expected");

        QTest::newRow("connected")    << QStringLiteral("Connected")    << true;
        QTest::newRow("recovery")     << QStringLiteral("Recovery")     << true;
        QTest::newRow("rescue")       << QStringLiteral("Rescue")       << true;
        QTest::newRow("lowercase")    << QStringLiteral("connected")    << true;
        QTest::newRow("spaces")       << QStringLiteral("  Recovery  ") << true;
        QTest::newRow("disconnected") << QStringLiteral("Disconnected") << false;
        QTest::newRow("offline")      << QStringLiteral("Offline")      << false;
        QTest::newRow("unauthorized") << QStringLiteral("Unauthorized") << false;
        QTest::newRow("sideload")     << QStringLiteral("Sideload")     << false;
        QTest::newRow("device")       << QStringLiteral("device")       << false;
        QTest::newRow("empty")        << QStringLiteral("")             << false;
    }

    void isConnectedStatusValues()
    {
        QFETCH(QString, status);
        QFETCH(bool, expected);
        QCOMPARE(isConnectedStatus(status), expected);
    }

    // --- parseTrackDevices ---
    void parseTrackDevicesSingle()
    {
        QByteArray buffer("0012HA1DB3G7\trecovery\n");
        const QList<QPair<QString, QString>> entries = parseTrackDevices(buffer);
        QCOMPARE(entries.size(), 1);
        QCOMPARE(entries.at(0).first, QStringLiteral("HA1DB3G7"));
        QCOMPARE(entries.at(0).second, QStringLiteral("recovery"));
        QVERIFY(buffer.isEmpty());
    }

    void parseTrackDevicesDeviceState()
    {
        const QByteArray payload("VHG02613012985\tdevice\n");
        QCOMPARE(payload.size(), 22);
        QByteArray buffer = QByteArray("0016") + payload;
        const QList<QPair<QString, QString>> entries = parseTrackDevices(buffer);
        QCOMPARE(entries.size(), 1);
        QCOMPARE(entries.at(0).first, QStringLiteral("VHG02613012985"));
        QCOMPARE(entries.at(0).second, QStringLiteral("device"));
        QVERIFY(buffer.isEmpty());
    }

    void parseTrackDevicesMultiDeviceHexPrefix()
    {
        // one chunk, two devices: the prefix is the whole payload length (0x2a),
        // so a digit-only strip would mangle the first serial to "aHA1DB3G7"
        const QByteArray payload("HA1DB3G7\trecovery\nVHG02613005678\trecovery\n");
        QCOMPARE(payload.size(), 42);
        QByteArray buffer = QByteArray("002a") + payload;

        const QList<QPair<QString, QString>> entries = parseTrackDevices(buffer);
        QCOMPARE(entries.size(), 2);
        QCOMPARE(entries.at(0).first, QStringLiteral("HA1DB3G7"));
        QCOMPARE(entries.at(0).second, QStringLiteral("recovery"));
        QCOMPARE(entries.at(1).first, QStringLiteral("VHG02613005678"));
        QCOMPARE(entries.at(1).second, QStringLiteral("recovery"));
        QVERIFY(buffer.isEmpty());
    }

    void parseTrackDevicesKeepsLeadingDigits()
    {
        QByteArray buffer("00101234abcd\tdevice\n");
        const QList<QPair<QString, QString>> entries = parseTrackDevices(buffer);
        QCOMPARE(entries.size(), 1);
        QCOMPARE(entries.at(0).first, QStringLiteral("1234abcd"));
        QCOMPARE(entries.at(0).second, QStringLiteral("device"));
        QVERIFY(buffer.isEmpty());
    }

    void parseTrackDevicesEmptyList()
    {
        QByteArray buffer("0000");
        QCOMPARE(parseTrackDevices(buffer).size(), 0);
        QVERIFY(buffer.isEmpty());
    }

    void parseTrackDevicesPartialFrame()
    {
        const QByteArray frame("0012HA1DB3G7\trecovery\n");
        QByteArray buffer = frame.left(10);
        QCOMPARE(parseTrackDevices(buffer).size(), 0);
        QCOMPARE(buffer.size(), 10);

        buffer.append(frame.mid(10));
        const QList<QPair<QString, QString>> entries = parseTrackDevices(buffer);
        QCOMPARE(entries.size(), 1);
        QCOMPARE(entries.at(0).first, QStringLiteral("HA1DB3G7"));
        QVERIFY(buffer.isEmpty());
    }

    void parseTrackDevicesIncompleteTrailer()
    {
        QByteArray buffer("0012HA1DB3G7\trecovery\n0012");
        const QList<QPair<QString, QString>> entries = parseTrackDevices(buffer);
        QCOMPARE(entries.size(), 1);
        QCOMPARE(buffer, QByteArray("0012"));
    }

    void parseTrackDevicesResync()
    {
        QByteArray buffer("zzzz0012HA1DB3G7\trecovery\n");
        const QList<QPair<QString, QString>> entries = parseTrackDevices(buffer);
        QCOMPARE(entries.size(), 1);
        QCOMPARE(entries.at(0).first, QStringLiteral("HA1DB3G7"));
        QVERIFY(buffer.isEmpty());
    }

    void parseTrackDevicesBogusLengthResync()
    {
        // "074T" used to parse as a length (0x74) because toInt stops at the first
        // character that is not a hex digit, shifting every later frame
        QByteArray buffer("074T0012HA1DB3G7\trecovery\n");
        const QList<QPair<QString, QString>> entries = parseTrackDevices(buffer);
        QCOMPARE(entries.size(), 1);
        QCOMPARE(entries.at(0).first, QStringLiteral("HA1DB3G7"));
        QCOMPARE(entries.at(0).second, QStringLiteral("recovery"));
        QVERIFY(buffer.isEmpty());
    }

    void parseTrackDevicesCarriageReturns()
    {
        // Windows adb writes the relayed list in text mode: the header counts LF
        // bytes, the wire carries CRLF
        QByteArray buffer("0016VHG02613012985\tdevice\r\n");
        const QList<QPair<QString, QString>> entries = parseTrackDevices(buffer);
        QCOMPARE(entries.size(), 1);
        QCOMPARE(entries.at(0).first, QStringLiteral("VHG02613012985"));
        QCOMPARE(entries.at(0).second, QStringLiteral("device"));
        QVERIFY(buffer.isEmpty());
    }

    void parseTrackDevicesCarriageReturnSplitAcrossReads()
    {
        const QByteArray frame("0012HA1DB3G7\trecovery\r\n");
        QByteArray buffer = frame.left(frame.size() - 1);
        QCOMPARE(parseTrackDevices(buffer).size(), 0);

        buffer.append(frame.right(1));
        const QList<QPair<QString, QString>> entries = parseTrackDevices(buffer);
        QCOMPARE(entries.size(), 1);
        QCOMPARE(entries.at(0).first, QStringLiteral("HA1DB3G7"));
        QVERIFY(buffer.isEmpty());
    }

    void parseTrackDevicesModeChangesWithCarriageReturns()
    {
        // device -> gone -> recovery -> gone -> device, as adb frames it on Windows
        const QByteArray device("G074TS0361670XW6\tdevice\r\n");
        const QByteArray recovery("G074TS0361670XW6\trecovery\r\n");
        QByteArray stream;
        stream += QByteArray("0018") + device;
        stream += QByteArray("0000");
        stream += QByteArray("001a") + recovery;
        stream += QByteArray("0000");
        stream += QByteArray("0018") + device;

        const QList<QPair<QString, QString>> entries = parseTrackDevices(stream);
        QCOMPARE(entries.size(), 3);
        for (const QPair<QString, QString> &entry : entries)
            QCOMPARE(entry.first, QStringLiteral("G074TS0361670XW6"));
        QCOMPARE(entries.at(0).second, QStringLiteral("device"));
        QCOMPARE(entries.at(1).second, QStringLiteral("recovery"));
        QCOMPARE(entries.at(2).second, QStringLiteral("device"));
        QVERIFY(stream.isEmpty());
    }
};

QTEST_MAIN(TestStringUtils)
#include "tst_stringutils.moc"
