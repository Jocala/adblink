#include <QtTest>
#include "adbstring.h"

class TestAdbString : public QObject
{
    Q_OBJECT

private slots:
    void plainString()
    {
        QCOMPARE(adbString("hello"), QStringLiteral("\"hello\""));
    }

    void withSpaces()
    {
        QCOMPARE(adbString("hello world"), QStringLiteral("\"hello world\""));
    }

    void withSingleQuote()
    {
        QCOMPARE(adbString("it's"), QStringLiteral("\"it\\'s\""));
    }

    void empty()
    {
        QCOMPARE(adbString(""), QStringLiteral("\"\""));
    }

    void alreadyHasDoubleQuotes()
    {
        QString result = adbString("already \"quoted\"");
        QVERIFY(result.startsWith('"'));
        QVERIFY(result.endsWith('"'));
    }
};

QTEST_MAIN(TestAdbString)
#include "tst_adbstring.moc"
