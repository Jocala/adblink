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
};

QTEST_MAIN(TestStringUtils)
#include "tst_stringutils.moc"
