#ifndef CREDENTIAL_H
#define CREDENTIAL_H

#include <QObject>

class Credential : public QObject {
    Q_OBJECT
   public:
    explicit Credential(QObject *parent = nullptr);
    QString username;
    QString password;
    QString host;
    QString payload;
    int port;
};

#endif  // CREDENTIAL_H
