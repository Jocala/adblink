#ifndef PREFERENCESMANAGER_H
#define PREFERENCESMANAGER_H

#include <QString>

class QWidget;

struct PreferencesResult {
    int lgFontIndex = 0;
    int mdFontIndex = 0;
    int smFontIndex = 0;
    int fmFontIndex = 0;
    QString donationValue;
};

class PreferencesManager
{
public:
    explicit PreferencesManager(const QString &configPath, int os);

    bool exec(QWidget *parent);

    PreferencesResult result() const { return m_result; }

private:
    QString m_configPath;
    int m_os;
    PreferencesResult m_result;
};

#endif // PREFERENCESMANAGER_H
