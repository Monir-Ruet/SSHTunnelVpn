#ifndef SOCKSPROXY_H
#define SOCKSPROXY_H

#include <QDebug>
#include <QNetworkProxy>
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include "sshworker.h"

class SocksProxy : public QTcpServer {
    Q_OBJECT
   public:
    explicit SocksProxy(QObject* parent = nullptr);
    void open_connection();

   signals:
    void on_client_data(QByteArray data);

   protected:
    void incomingConnection(qintptr socketDescriptor) override;

    QSettings settings;
};

#endif  // SOCKSPROXY_H
