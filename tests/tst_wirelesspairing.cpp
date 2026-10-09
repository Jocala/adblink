#include <QtTest>
#include <QTemporaryDir>
#include "wirelesspairing.h"

using namespace WirelessPairing;

static Request req(const QString &host, const QString &pport, const QString &code,
                   const QString &cport = QString())
{
    Request r;
    r.host = host;
    r.pairPort = pport;
    r.code = code;
    r.connectPort = cport;
    return r;
}

class TestWirelessPairing : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir m_tmp;
    QString m_fakeAdb;

    void writeFakeAdb(const QByteArray &script)
    {
        QFile f(m_fakeAdb);
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
        f.write(script);
        f.close();
        QFile::setPermissions(m_fakeAdb, QFile::permissions(m_fakeAdb) | QFileDevice::ExeOwner);
    }

private slots:
    void initTestCase()
    {
        QVERIFY(m_tmp.isValid());
        m_fakeAdb = m_tmp.path() + "/adb";
    }

    void validInput()
    {
        QVERIFY(validate(req("192.168.1.20", "37099", "123456")).isEmpty());
        QVERIFY(validate(req("192.168.1.20", "37099", "012345", "41235")).isEmpty());
        QVERIFY(validate(req(" tv.local ", "37099", "123456")).isEmpty());
    }

    void invalidInput()
    {
        QVERIFY(!validate(req("", "37099", "123456")).isEmpty());
        QVERIFY(!validate(req("192.168.1.300", "37099", "123456")).isEmpty());
        QVERIFY(!validate(req("192.168.1.20", "", "123456")).isEmpty());
        QVERIFY(!validate(req("192.168.1.20", "70000", "123456")).isEmpty());
        QVERIFY(!validate(req("192.168.1.20", "37099", "12345")).isEmpty());
        QVERIFY(!validate(req("192.168.1.20", "37099", "12345a")).isEmpty());
        QVERIFY(!validate(req("192.168.1.20", "37099", "123456", "0")).isEmpty());
    }

    void arguments()
    {
        const Request r = req(" 192.168.1.20 ", " 37099", "012345 ", "41235");
        QCOMPARE(pairArguments(r), (QStringList{ "pair", "192.168.1.20:37099", "012345" }));
        QCOMPARE(connectArguments(r), (QStringList{ "connect", "192.168.1.20:41235" }));
    }

    void outputParsing()
    {
        QVERIFY(pairSucceeded("Successfully paired to 192.168.1.20:37099 [guid=adb-3B1A-x9]"));
        QVERIFY(!pairSucceeded("Failed: Wrong password or connection was dropped."));
        QVERIFY(!pairSucceeded(""));
        QVERIFY(connectSucceeded("connected to 192.168.1.20:41235"));
        QVERIFY(connectSucceeded("already connected to 192.168.1.20:41235"));
        QVERIFY(!connectSucceeded("failed to connect to '192.168.1.20:41235': Connection refused"));
        QVERIFY(!connectSucceeded("cannot connect to 192.168.1.20:41235: No route to host"));
    }

    void failureMessages()
    {
        QVERIFY(describeFailure("Failed: Wrong password or connection was dropped.").contains("not accepted"));
        QVERIFY(describeFailure("Failed: Unable to start pairing client.").contains("pairing port"));
        QCOMPARE(describeFailure("adb: error: something odd"), QStringLiteral("something odd"));
        QVERIFY(!describeFailure("").isEmpty());
    }

    void runPairsAndConnects()
    {
        // Succeeds only if adb receives exactly the expected arguments
        writeFakeAdb("#!/bin/sh\n"
                     "if [ \"$1\" = pair ] && [ \"$2\" = 192.168.1.20:37099 ] && [ \"$3\" = 012345 ] && [ $# -eq 3 ]; then\n"
                     "  echo 'Successfully paired to 192.168.1.20:37099 [guid=adb-x]'; exit 0; fi\n"
                     "if [ \"$1\" = connect ] && [ \"$2\" = 192.168.1.20:41235 ] && [ $# -eq 2 ]; then\n"
                     "  echo 'connected to 192.168.1.20:41235'; exit 0; fi\n"
                     "echo \"Failed: unexpected args: $*\"; exit 1\n");
        const Result r = run(m_fakeAdb, req("192.168.1.20", "37099", "012345", "41235"));
        QVERIFY2(r.paired, qPrintable(r.pairOutput));
        QVERIFY2(r.connected, qPrintable(r.connectOutput));
    }

    void runSkipsConnectWithoutPort()
    {
        writeFakeAdb("#!/bin/sh\n[ \"$1\" = pair ] && echo 'Successfully paired to x' && exit 0\n"
                     "echo 'connect should not run'; exit 1\n");
        const Result r = run(m_fakeAdb, req("192.168.1.20", "37099", "123456"));
        QVERIFY(r.paired);
        QVERIFY(!r.connected);
        QVERIFY(r.connectOutput.isEmpty());
    }

    void runDoesNotConnectAfterFailedPair()
    {
        writeFakeAdb("#!/bin/sh\n[ \"$1\" = pair ] && echo 'Failed: Wrong password or connection was dropped.' && exit 1\n"
                     "echo 'connected to x'; exit 0\n");
        const Result r = run(m_fakeAdb, req("192.168.1.20", "37099", "123456", "41235"));
        QVERIFY(!r.paired);
        QVERIFY(r.connectOutput.isEmpty());
    }

    void runTimesOut()
    {
        writeFakeAdb("#!/bin/sh\nexec sleep 30\n");
        QElapsedTimer t;
        t.start();
        const Result r = run(m_fakeAdb, req("192.168.1.20", "37099", "123456"), 500);
        QVERIFY(!r.paired);
        QVERIFY(t.elapsed() < 5000);
        QVERIFY(r.pairOutput.contains("timed out"));
    }

    void runMissingAdb()
    {
        const Result r = run(m_tmp.path() + "/no-such-adb", req("192.168.1.20", "37099", "123456"));
        QVERIFY(!r.paired);
        QVERIFY(r.pairOutput.contains("could not start adb"));
    }
};

QTEST_MAIN(TestWirelessPairing)
#include "tst_wirelesspairing.moc"
