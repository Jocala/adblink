#include <QtTest>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include "devicerecorddialog.h"

class TestDeviceRecordDialog : public QObject
{
    Q_OBJECT

private:
    DeviceRecordDialog *dialog = nullptr;

    QRadioButton *findRadio(const QString &text) const
    {
        const auto radios = dialog->findChildren<QRadioButton *>();
        for (auto *r : radios) {
            if (r->text() == text)
                return r;
        }
        return nullptr;
    }

    QCheckBox *findCheckBox(const QString &text) const
    {
        const auto checks = dialog->findChildren<QCheckBox *>();
        for (auto *c : checks) {
            if (c->text() == text)
                return c;
        }
        return nullptr;
    }

private slots:
    void init()
    {
        dialog = new DeviceRecordDialog(nullptr, true);
    }

    void cleanup()
    {
        delete dialog;
        dialog = nullptr;
    }

    void defaultState()
    {
        QCOMPARE(dialog->windowTitle(), QStringLiteral("Device Record"));
        QCOMPARE(dialog->ostype(), QStringLiteral("0"));
        QCOMPARE(dialog->daddr(), QString());
        QCOMPARE(dialog->port(), QStringLiteral("5555"));
        QCOMPARE(dialog->description(), QString());
        QCOMPARE(dialog->xbmcpackageName(), QString());
        QCOMPARE(dialog->pulldir(), QString());
        QCOMPARE(dialog->filepath(), QString());
        QCOMPARE(dialog->scrcpy(), QString());
        QCOMPARE(dialog->data_root(), QString());
        QVERIFY(!dialog->isusb());
        QVERIFY(!dialog->scoped());
        QVERIFY(!dialog->wsa());
        QVERIFY(!dialog->disableroot());
    }

    void constructorWithoutKodi()
    {
        DeviceRecordDialog d(nullptr, false);
        QCOMPARE(d.windowTitle(), QStringLiteral("Device Record"));
        QCOMPARE(d.ostype(), QStringLiteral("0"));
    }

    void stringFieldRoundTrips()
    {
        dialog->setdaddr(QStringLiteral("192.168.1.100"));
        QCOMPARE(dialog->daddr(), QStringLiteral("192.168.1.100"));

        dialog->setdescription(QStringLiteral("Living Room"));
        QCOMPARE(dialog->description(), QStringLiteral("Living Room"));

        dialog->setfilepath(QStringLiteral("files/.kodi"));
        QCOMPARE(dialog->filepath(), QStringLiteral("files/.kodi"));

        dialog->setscrcpy(QStringLiteral("--max-size=1080"));
        QCOMPARE(dialog->scrcpy(), QStringLiteral("--max-size=1080"));

        dialog->setdataroot(QStringLiteral("/sdcard/"));
        QCOMPARE(dialog->data_root(), QStringLiteral("/sdcard/"));

        dialog->setport(QStringLiteral("5555"));
        QCOMPARE(dialog->port(), QStringLiteral("5555"));

        dialog->setPackagename(QStringLiteral("org.xbmc.kodi"));
        QCOMPARE(dialog->xbmcpackageName(), QStringLiteral("org.xbmc.kodi"));

        dialog->setPulldir(QStringLiteral("/sdcard/Download"));
        QCOMPARE(dialog->pulldir(), QStringLiteral("/sdcard/Download"));
    }

    void booleanFieldRoundTrips()
    {
        dialog->setisusb(true);
        QVERIFY(dialog->isusb());
        dialog->setisusb(false);
        QVERIFY(!dialog->isusb());

        dialog->setscope(true);
        QVERIFY(dialog->scoped());
        dialog->setscope(false);
        QVERIFY(!dialog->scoped());

        dialog->setwsa(true);
        QVERIFY(dialog->wsa());
        dialog->setwsa(false);
        QVERIFY(!dialog->wsa());

        dialog->setdisableroot(true);
        QVERIFY(dialog->disableroot());
        dialog->setdisableroot(false);
        QVERIFY(!dialog->disableroot());
    }

    void ostypeRoundTrip()
    {
        // Combo box has items 0-3 (Android/Windows/macOS/Linux)
        for (int i = 0; i <= 3; ++i) {
            dialog->setostype(QString::number(i));
            QCOMPARE(dialog->ostype(), QString::number(i));
        }
    }

    void radioButtonKodiSetsDefaults()
    {
        QRadioButton *kodi = findRadio(QStringLiteral("Kodi"));
        QVERIFY2(kodi, "Could not find 'Kodi' radio button");
        kodi->click();
        QCOMPARE(dialog->xbmcpackageName(), QStringLiteral("org.xbmc.kodi"));
        QCOMPARE(dialog->filepath(), QStringLiteral("/files/.kodi"));
    }

    void radioButtonSpmcSetsDefaults()
    {
        QRadioButton *spmc = findRadio(QStringLiteral("SPMC"));
        QVERIFY2(spmc, "Could not find 'SPMC' radio button");
        spmc->click();
        QCOMPARE(dialog->xbmcpackageName(), QStringLiteral("com.semperpax.spmc16"));
        QCOMPARE(dialog->filepath(), QStringLiteral("/files/.spmc"));
    }

    void radioButtonOtherClearsFields()
    {
        dialog->setPackagename(QStringLiteral("org.xbmc.kodi"));
        dialog->setfilepath(QStringLiteral("files/.kodi"));

        QRadioButton *other = findRadio(QStringLiteral("Other"));
        QVERIFY2(other, "Could not find 'Other' radio button");
        other->click();
        QCOMPARE(dialog->xbmcpackageName(), QString());
        QCOMPARE(dialog->filepath(), QString());
    }

    void usbCheckboxClearsPort()
    {
        QCheckBox *usb = findCheckBox(QStringLiteral("USB Connection"));
        QVERIFY2(usb, "Could not find 'USB Connection' checkbox");
        QCOMPARE(dialog->port(), QStringLiteral("5555"));
        usb->click();
        QCOMPARE(dialog->port(), QString());
        usb->click();
        QCOMPARE(dialog->port(), QStringLiteral("5555"));
    }

    void osTypeAndroidSetsDefaults()
    {
        QComboBox *combo = dialog->findChild<QComboBox *>();
        QVERIFY(combo);
        combo->setCurrentIndex(0);
        QCOMPARE(dialog->ostype(), QStringLiteral("0"));
        QCOMPARE(dialog->port(), QStringLiteral("5555"));
    }

    void wsaCheckboxTogglesFields()
    {
        QCheckBox *wsa = findCheckBox(QStringLiteral("WSA"));
        QVERIFY2(wsa, "Could not find 'WSA' checkbox");
        wsa->click();
        QCOMPARE(dialog->daddr(), QStringLiteral("127.0.0.1"));
        QCOMPARE(dialog->port(), QStringLiteral("58526"));
        QCOMPARE(dialog->data_root(), QStringLiteral("/sdcard/"));
        QCOMPARE(dialog->filepath(), QStringLiteral("kodi_data/files/.kodi"));
        QVERIFY(dialog->scoped());

        wsa->click();
        QCOMPARE(dialog->daddr(), QString());
        QCOMPARE(dialog->port(), QStringLiteral("5555"));
        QCOMPARE(dialog->data_root(), QStringLiteral("/sdcard/"));
        QCOMPARE(dialog->filepath(), QStringLiteral("/files/.kodi"));

    }

    void scopedCheckboxTogglesPaths()
    {
        QCheckBox *scoped = findCheckBox(QStringLiteral("SS Override"));
        QVERIFY2(scoped, "Could not find 'SS Override' checkbox");
        scoped->click();
        QCOMPARE(dialog->data_root(), QStringLiteral("/sdcard/"));
        QCOMPARE(dialog->filepath(), QStringLiteral("kodi_data/files/.kodi"));

        scoped->click();
        QCOMPARE(dialog->filepath(), QStringLiteral("/files/.kodi"));
        QCOMPARE(dialog->data_root(), QStringLiteral("/sdcard/"));
    }
};

QTEST_MAIN(TestDeviceRecordDialog)
#include "tst_devicerecorddialog.moc"
