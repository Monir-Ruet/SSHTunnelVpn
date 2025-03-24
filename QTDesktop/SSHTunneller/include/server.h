#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <iostream>
#include "socket.h"
#include <QDebug>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void setupServer();
    void socks5Handler(Socket *remote);
signals:
private:
    Socket * socket;
};

#endif // SERVER_H
