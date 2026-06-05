#include <QtTest>
#include "getreturncode.h"

class TestGetReturnCode : public QObject
{
    Q_OBJECT

private slots:
    void exitZero()
    {
#ifdef Q_OS_WIN
        QVERIFY(getreturncode(QStringLiteral("cmd /c exit 0")));
#else
        QVERIFY(getreturncode(QStringLiteral("true")));
#endif
    }

    void exitNonZero()
    {
#ifdef Q_OS_WIN
        QVERIFY(!getreturncode(QStringLiteral("cmd /c exit 1")));
#else
        QVERIFY(!getreturncode(QStringLiteral("false")));
#endif
    }

    void emptyCommand()
    {
        QVERIFY(!getreturncode(QString()));
    }

    void commandNotFound()
    {
        QVERIFY(!getreturncode(QStringLiteral("nonexistent_command_xyz_12345")));
    }
};

QTEST_APPLESS_MAIN(TestGetReturnCode)
#include "tst_getreturncode.moc"
