#ifndef OCULUSMANAGER_H
#define OCULUSMANAGER_H

#include <QString>

class QWidget;

class OculusManager {
public:
    OculusManager(const QString &adbPrefix, const QString &dbDir, QWidget *parent = nullptr);
    void exec();

private:
    void readOrCreateConfig();
    void saveConfig();
    void applySettings(class oculusDialog &dialog, bool &execute_true);

    QString m_adbPrefix;
    QString m_dbDir;
    QWidget *m_parent;

    int m_power = 0;
    int m_proximity = 0;
    int m_guardian = 0;
    int m_cpu = 0;
    int m_gpu = 0;
    int m_refresh = 0;
    int m_texture = 0;
    int m_recording = 0;
    int m_fovlevel = 0;
    int m_dynamicfov = 0;
    int m_ratecap = 0;
    int m_chromatic = 0;
    int m_exper = 0;

    QString m_customCaptureWidth;
    QString m_customCaptureHeight;
    QString m_customTextureWidth;
    QString m_customTextureHeight;
    QString m_customBitrate;
    QString m_fovdown;
    QString m_fovup;
    QString m_fovout;
    QString m_fovin;
};

#endif
