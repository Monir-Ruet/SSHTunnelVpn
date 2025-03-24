#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMutex>
#include <QObject>
#include <QSettings>
#include "credential.h"

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);
    static Settings* instance();
    void saveCredentials(Credential* credential);
    Credential* getCredentialSetting();
    QString getSetting(QString key);
private:
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    static Settings* m_instance;
    static QMutex mutex;
    QSettings settings;
};

#endif // SETTINGS_H
