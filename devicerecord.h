#pragma once
#include <QString>

struct DeviceRecord {
    int id = 0;
    QString daddr;
    QString pulldir;
    QString xbmcpackage = "org.xbmc.kodi";
    QString data_root = "/sdcard/";
    int buffermode = 1;
    QString buffersize;
    QString bufferfactor;
    QString description;
    QString filepath = "files/.kodi";
    QString port;
    bool isusb = false;
    bool disableroot = false;
    bool scoped = false; // flag1
    QString comments;    // flag2 (was wsa)
    QString ostype = "0"; // Default after validation
    QString scrcpyarg; // flag5
};
