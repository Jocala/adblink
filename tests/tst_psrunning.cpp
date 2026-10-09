#include <QtTest>
#include "adbutils.h"

// Samples trimmed from real devices
static const char *kToyboxPsA =
    "USER           PID  PPID     VSZ    RSS WCHAN            ADDR S NAME\n"
    "root             1     0 2133504  12212 0                   0 S init\n"
    "u0_a87        4321   612 2914028 210340 0                   0 S org.xbmc.kodi\n"
    "u0_a87        4400   612 1920000  40000 0                   0 S org.xbmc.kodi:service\n";

static const char *kToolboxPs =
    "USER     PID   PPID  VSIZE  RSS     WCHAN    PC        NAME\r\n"
    "root      1     0     1036   576   c01b4e98 0001bb48 S /init\r\n"
    "u0_a52    2210  198   1210700 180344 ffffffff b6f4c6c4 S org.xbmc.kodi\r\n";

class TestPsRunning : public QObject
{
    Q_OBJECT

private slots:
    void toyboxAndroid8Plus() { QVERIFY(psOutputHasPackage(kToyboxPsA, "org.xbmc.kodi")); }
    void toolboxOldAndroid()   { QVERIFY(psOutputHasPackage(kToolboxPs, "org.xbmc.kodi")); }
    void serviceProcessOnly()
    {
        QVERIFY(psOutputHasPackage("u0_a87 4400 612 1 1 0 0 S org.xbmc.kodi:service\n", "org.xbmc.kodi"));
    }
    void notRunning()          { QVERIFY(!psOutputHasPackage(kToyboxPsA, "net.kodi.fork")); }
    void prefixOfOtherPackage()
    {
        // org.xbmc.kodi must not match a fork named org.xbmc.kodi.beta
        QVERIFY(!psOutputHasPackage("u0_a90 5000 612 1 1 0 0 S org.xbmc.kodi.beta\n", "org.xbmc.kodi"));
    }
    void grepLineIgnored()
    {
        // Android ps shows only the process name, so the grep itself shows as "grep"
        QVERIFY(!psOutputHasPackage("shell 6000 5999 1 1 0 0 S grep\n", "org.xbmc.kodi"));
    }
    void emptyOutput()         { QVERIFY(!psOutputHasPackage(QString(), "org.xbmc.kodi")); }
    void emptyPackage()        { QVERIFY(!psOutputHasPackage(kToyboxPsA, QString())); }
    void adbError()
    {
        QVERIFY(!psOutputHasPackage("error: device 'x' not found\n", "org.xbmc.kodi"));
    }
};

QTEST_APPLESS_MAIN(TestPsRunning)
#include "tst_psrunning.moc"
