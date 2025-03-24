#include "settings.h"

Settings* Settings::m_instance = nullptr;
QMutex Settings::mutex;

Settings::Settings(QObject *parent)
    : QObject{parent}
{
}

Settings *Settings::instance()
{
    QMutexLocker locker(&mutex);

    if(m_instance == nullptr){
        m_instance = new Settings();
    }

    return m_instance;
}

QString Settings::getSetting(QString key){
    return settings.value(key).toString();
}

void Settings::saveCredentials(Credential *cred)
{
    if(cred->username.size() == 0) return;
    if(cred->host.size() == 0) return;
    if(cred->password.size() == 0) return;
    if(cred->payload.size() == 0) return;
    if(cred->port <= 0 && cred->port >= 65536) return;
    settings.setValue("username", cred->username);
    settings.setValue("password", cred->password);
    settings.setValue("host", cred->host);
    settings.setValue("payload", cred->payload);
    settings.setValue("port", cred->port);
}


Credential* Settings::getCredentialSetting(){
    Credential* credential = new Credential();
    credential->username = settings.value("name").toString();
    credential->password = settings.value("password").toString();
    credential->payload = settings.value("payload").toString();
    credential->port = settings.value("port").toInt();
    credential->payload = settings.value("payload").toString();
    return credential;
}
