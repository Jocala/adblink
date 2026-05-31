#include "deviceeditor.h"
#include "devicerecorddialog.h"
#include "logfile.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

DeviceEditor::DeviceEditor(QWidget *parent, bool iskodi,
                         const QString &version,
                         std::function<void()> eraseCallback)
    : QObject(parent)
    , m_parent(parent)
    , m_iskodi(iskodi)
    , m_version(version)
    , m_eraseCallback(std::move(eraseCallback))
{
}

void DeviceEditor::setDeviceList(const QStringList &devices)
{
    m_devices = devices;
}

void DeviceEditor::setExistingDevice(const DeviceRecord &device, const QString &description)
{
    m_existingDevice = device;
    m_existingDescription = description;
    m_isUpdate = true;
}

bool DeviceEditor::exec()
{
    DeviceRecordDialog dialog(m_parent, m_iskodi);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setversionLabel(m_version);
    dialog.setdevicelist(m_devices);

    if (m_isUpdate)
    {
        dialog.setPackagename(m_existingDevice.xbmcpackage);
        dialog.setPulldir(m_existingDevice.pulldir);
        dialog.setfilepath(m_existingDevice.filepath);
        dialog.setscrcpy(m_existingDevice.scrcpyarg);
        dialog.setdataroot(m_existingDevice.data_root);
        dialog.setostype("0");
        dialog.setdescription(m_existingDevice.description);
        dialog.setdisableroot(m_existingDevice.disableroot);
        if (m_existingDevice.isusb)
            dialog.setport("");
        else
            dialog.setport(m_existingDevice.port);
        dialog.setdaddr(m_existingDevice.daddr);
        dialog.setisusb(m_existingDevice.isusb);
    }
    else
    {
        dialog.setPackagename("org.xbmc.kodi");
        dialog.setPulldir("");
        dialog.setfilepath("/files/.kodi");
        dialog.setdataroot("/sdcard/");
        dialog.setostype("0");
        dialog.setdescription("");
        dialog.setscrcpy("");
        dialog.setdisableroot(0);
        dialog.setport("5555");
        dialog.setscope(false);
        dialog.setwsa(false);
        dialog.setdaddr("");
        dialog.setisusb(false);
    }

    dialog.setModal(true);

    int result = dialog.exec();
    if (result != QDialog::Accepted)
        return false;

    QString data_root = dialog.data_root();
    QString xbmcpackage = dialog.xbmcpackageName();
    QString pulldir = dialog.pulldir();
    QString description = dialog.description();
    QString filepath = dialog.filepath();
    QString port = dialog.port();
    QString daddr = dialog.daddr();
    bool isusb = dialog.isusb();
    QString ostype = dialog.ostype();
    int disableroot = dialog.disableroot();
    QString scrcpy = dialog.scrcpy();

    if (description.isEmpty())
    {
        QMessageBox::critical(m_parent, "", "Description cannot be empty.");
        return false;
    }

    if (!m_isUpdate)
    {
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT COUNT(*) FROM device WHERE description = ?");
        checkQuery.addBindValue(description);
        if (checkQuery.exec() && checkQuery.first() && checkQuery.value(0).toInt() > 0)
        {
            QMessageBox::critical(m_parent, "", "A device with this description already exists.");
            return false;
        }
    }

    QSqlQuery query;
    QString sqlstatement;

    if (!m_isUpdate)
    {
        sqlstatement = "INSERT INTO device (description, daddr, port, isusb, ostype, "
                       "data_root, xbmcpackage, pulldir, disableroot, filepath, flag5) "
                       "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        query.prepare(sqlstatement);
        query.addBindValue(description);
        query.addBindValue(daddr);
        query.addBindValue(port);
        query.addBindValue(isusb);
        query.addBindValue(ostype);
        query.addBindValue(data_root);
        query.addBindValue(xbmcpackage);
        query.addBindValue(pulldir);
        query.addBindValue(disableroot);
        query.addBindValue(filepath);
        query.addBindValue(scrcpy);
    }
    else
    {
        sqlstatement = "UPDATE device SET description = ?, daddr = ?, port = ?, isusb = ?, ostype = ?, "
                       "data_root = ?, xbmcpackage = ?, pulldir = ?, disableroot = ?, filepath = ?, flag5 = ? "
                       "WHERE description = ?";
        query.prepare(sqlstatement);
        query.addBindValue(description);
        query.addBindValue(daddr);
        query.addBindValue(port);
        query.addBindValue(isusb);
        query.addBindValue(ostype);
        query.addBindValue(data_root);
        query.addBindValue(xbmcpackage);
        query.addBindValue(pulldir);
        query.addBindValue(disableroot);
        query.addBindValue(filepath);
        query.addBindValue(scrcpy);
        query.addBindValue(m_existingDescription);
    }

    if (!query.exec())
    {
        QString errorMessage = query.lastError().text();
        logfile(QString("Query error: ") + errorMessage);
        logfile(QString("SQL statement: ") + sqlstatement);
        logfile(QString("Bound values: description=%1, daddr=%2, port=%3, isusb=%4, ostype=%5, data_root=%6, xbmcpackage=%7, pulldir=%8, disableroot=%9, filepath=%10, scrcpy=%11")
                    .arg(description)
                    .arg(daddr)
                    .arg(port)
                    .arg(isusb ? "true" : "false")
                    .arg(ostype)
                    .arg(data_root)
                    .arg(xbmcpackage)
                    .arg(pulldir)
                    .arg(disableroot)
                    .arg(filepath)
                    .arg(scrcpy));

        if (errorMessage.contains("Parameter count mismatch"))
        {
            QMessageBox msgBox(m_parent);
            msgBox.setWindowTitle("Parameter Mismatch Error");
            msgBox.setText("Parameter count mismatch detected. Would you like to re-initialize adblink?");
            msgBox.setIcon(QMessageBox::Question);

            QAbstractButton *yesButton = msgBox.addButton(QMessageBox::Yes);
            QAbstractButton *noButton = msgBox.addButton(QMessageBox::No);

            bool done = false;
            while (!done)
            {
                msgBox.exec();
                QAbstractButton *clicked = msgBox.clickedButton();
                if (clicked == yesButton)
                {
                    if (m_eraseCallback)
                        m_eraseCallback();
                    done = true;
                }
                else if (clicked == noButton)
                {
                    done = true;
                }
            }
            return false;
        }
        else
        {
            QMessageBox::critical(m_parent, "",
                                  (!m_isUpdate ? "Failed to insert into database: " : "Failed to update database: ") + errorMessage);
            return false;
        }
    }
    else
    {
        logfile(QString("Query executed successfully for ") + (!m_isUpdate ? "INSERT" : "UPDATE"));
    }

    QSqlDatabase::database().commit();
    return true;
}
