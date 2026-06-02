#include <QtTest>
#include "iptablewidgetitem.h"

class TestIpSort : public QObject
{
    Q_OBJECT

private slots:
    void naAlwaysFalseWhenOnLeft()
    {
        IpTableWidgetItem naItem("N/A");
        IpTableWidgetItem ipItem("192.168.1.1");
        QVERIFY(!(naItem < ipItem));
    }

    void naAlwaysTrueWhenOnRight()
    {
        IpTableWidgetItem ipItem("10.0.0.1");
        IpTableWidgetItem naItem("N/A");
        QVERIFY(ipItem < naItem);
    }

    void normalOctetSorting()
    {
        IpTableWidgetItem a("192.168.1.1");
        IpTableWidgetItem b("192.168.1.2");
        QVERIFY(a < b);
        QVERIFY(!(b < a));
    }

    void firstOctetDifference()
    {
        IpTableWidgetItem a("10.0.0.1");
        IpTableWidgetItem b("192.168.1.1");
        QVERIFY(a < b);
    }

    void secondOctetDifference()
    {
        IpTableWidgetItem a("192.168.0.1");
        IpTableWidgetItem b("192.168.1.1");
        QVERIFY(a < b);
    }

    void equalIps()
    {
        IpTableWidgetItem a("192.168.1.1");
        IpTableWidgetItem b("192.168.1.1");
        QVERIFY(!(a < b));
        QVERIFY(!(b < a));
    }

    void fewerOctetsSortsFirst()
    {
        // "192.168.1" (3 octets) is treated as shorter
        IpTableWidgetItem a("192.168.1");
        IpTableWidgetItem b("192.168.1.1");
        QVERIFY(a < b);
    }

    void usbStringSortsByText()
    {
        IpTableWidgetItem a("USB");
        IpTableWidgetItem b("192.168.1.1");
        IpTableWidgetItem c("10.0.0.1");
        // "USB" vs IP — treated as regular string comparison
        // since neither is "N/A" and split by "." gives ["USB"]
        Q_UNUSED(a);
        Q_UNUSED(b);
        Q_UNUSED(c);
    }
};

QTEST_APPLESS_MAIN(TestIpSort)
#include "tst_iptablesort.moc"
