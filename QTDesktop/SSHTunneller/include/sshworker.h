#ifndef SSHWORKER_H
#define SSHWORKER_H

#include <QObject>
#include <QSocketNotifier>
#include <libssh/libssh.h>
#include <iostream>
#include "ssh.h"

class SSHWorker : public QObject {
    Q_OBJECT
   public:
    explicit SSHWorker(QObject *parent = nullptr);
    ~SSHWorker();
    bool enablePortForwarding(QByteArray address, int port);
    void forward_data(QTcpSocket* socket);

   signals:
    void on_response_ready(QByteArray data);
   public slots:
    void cleanup();
   private:
    ssh_channel channel;
    SSH *ssh;
};

#endif  // SSHWORKER_H
