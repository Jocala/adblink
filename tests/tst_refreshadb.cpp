#include <QtTest>
#include "refreshadbmanager.h"

class TestRefreshAdb : public QObject
{
    Q_OBJECT

private slots:
    void parsesDeviceLines()
    {
        const QString output = QStringLiteral("List of devices attached\n"
                                              "192.168.1.101:5555\tdevice\n"
                                              "VHG02613012985\tdevice\n");
        const QHash<QString, QString> result = RefreshAdbManager::parseAdbDevices(output);
        QCOMPARE(result.size(), 2);
        QCOMPARE(result.value(QStringLiteral("192.168.1.101:5555")), QStringLiteral("device"));
        QCOMPARE(result.value(QStringLiteral("VHG02613012985")), QStringLiteral("device"));
    }

    void keepsNonDeviceStates()
    {
        const QString output = QStringLiteral("List of devices attached\n"
                                              "192.168.1.101:5555\toffline\n"
                                              "VHG02613012985\tunauthorized\n");
        const QHash<QString, QString> result = RefreshAdbManager::parseAdbDevices(output);
        QCOMPARE(result.value(QStringLiteral("192.168.1.101:5555")), QStringLiteral("offline"));
        QCOMPARE(result.value(QStringLiteral("VHG02613012985")), QStringLiteral("unauthorized"));
    }

    void skipsHeaderAndBlanks()
    {
        const QString output = QStringLiteral("List of devices attached\n\n\n");
        QVERIFY(RefreshAdbManager::parseAdbDevices(output).isEmpty());
    }

    void skipsDaemonChatter()
    {
        const QString output = QStringLiteral("* daemon not running; starting now at tcp:5037\n"
                                              "* daemon started successfully\n"
                                              "List of devices attached\n"
                                              "192.168.1.101:5555\tdevice\n");
        const QHash<QString, QString> result = RefreshAdbManager::parseAdbDevices(output);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.value(QStringLiteral("192.168.1.101:5555")), QStringLiteral("device"));
    }

    void emptyOutputParsesEmpty()
    {
        QVERIFY(RefreshAdbManager::parseAdbDevices(QString()).isEmpty());
    }

    void preservesFullIpSerial()
    {
        const QString output = QStringLiteral("List of devices attached\n192.168.1.101:37123\tdevice\n");
        const QHash<QString, QString> result = RefreshAdbManager::parseAdbDevices(output);
        QVERIFY(result.contains(QStringLiteral("192.168.1.101:37123")));
    }
};

QTEST_MAIN(TestRefreshAdb)
#include "tst_refreshadb.moc"
