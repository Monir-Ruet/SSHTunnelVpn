#include "socksproxy.h"
#include "socketthread.h"

SocksProxy::SocksProxy(QObject *parent) : QTcpServer(parent) {
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    SSH *ssh = SSH::instance();
    if(ssh->connectToServer()){
        qDebug() << "Connected\n";
    }else{
        qDebug() << "Error in connecting to the server";
    }
}

void SocksProxy::incomingConnection(qintptr socketDescriptor) {
    SocketThread *socket = new SocketThread(socketDescriptor);
    connect(socket, SIGNAL(finished()), socket, SLOT(deleteLater()));
    socket->start();

    // qDebug() << "New Client Connected";
    // QTcpSocket *client = new QTcpSocket(this);
    // client->setSocketDescriptor(socketDescriptor);

    // client->waitForReadyRead();
    // QByteArray data = client->read(2);
    // if (data.size() < 2 || static_cast<uint8_t>(data[0]) != 0x05) {
    //     qDebug() << "Invalid SOCKS5 version or incomplete greeting.";
    //     return;
    // }

    // uint8_t numMethods = static_cast<uint8_t>(data[1]);
    // if (client->bytesAvailable() < numMethods) {
    //     return; // Wait for more data
    // }

    // QByteArray methods = client->read(numMethods);
    // bool noAuthSupported = methods.contains(0x00);

    // if (!noAuthSupported) {
    //     qDebug() << "No supported authentication methods.";
    //     QByteArray response;
    //     response.append(0x05); // SOCKS5 version
    //     response.append(0xFF); // No acceptable methods
    //     client->write(response);
    //     return;
    // }

    // // Respond with "no authentication required" (0x00)
    // QByteArray response;
    // response.append(0x05); // SOCKS5 version
    // response.append(static_cast<char>(0x00)); // No authentication
    // client->write(response);

    // client->waitForReadyRead();

    // data = client->read(4);
    // if (data.size() < 4 || static_cast<uint8_t>(data[0]) != 0x05 || static_cast<uint8_t>(data[1]) != 0x01) {
    //     qDebug() << "Invalid SOCKS5 request.";
    //     return;
    // }

    // uint8_t addressType = static_cast<uint8_t>(data[3]);
    // QByteArray address;
    // quint16 port;

    // if (addressType == 0x01) { // IPv4 address
    //     if (client->bytesAvailable() < 4 + 2) {
    //         return; // Wait for more data
    //     }
    //     address = client->read(4);
    //     port = (static_cast<quint8>(client->read(1)[0]) << 8) + static_cast<quint8>(client->read(1)[0]);
    // } else if (addressType == 0x03) { // Domain name
    //     if (client->bytesAvailable() < 1) {
    //         return; // Wait for more data
    //     }
    //     uint8_t domainLength = static_cast<uint8_t>(client->read(1)[0]);
    //     if (client->bytesAvailable() < domainLength + 2) {
    //         return; // Wait for more data
    //     }
    //     address = client->read(domainLength);
    //     port = (static_cast<quint8>(client->read(1)[0]) << 8) + static_cast<quint8>(client->read(1)[0]);
    // } else {
    //     qDebug() << "Unsupported address type:" << addressType;
    //     return;
    // }

    // // Send success response
    // QByteArray successResponse;
    // successResponse.append(char(5)); // SOCKS5 version
    // successResponse.append(char(0)); // Success
    // successResponse.append(char(0)); // Reserved
    // successResponse.append(char(1)); // IPv4 address type
    // successResponse.append(char(0)); // Placeholder for IPv4 address (0.0.0.0)
    // successResponse.append(char(0));
    // successResponse.append(char(0));
    // successResponse.append(char(0));
    // successResponse.append(char((port >> 8) & 0xFF)); // High byte
    // successResponse.append(char(port & 0xFF));        // Low byte
    // client->write(successResponse);

    // // Create SSHWorker for port forwarding
    // SSHWorker *sshWorker = new SSHWorker();
    // if (!sshWorker->enablePortForwarding(address, port)) {
    //     qDebug() << "Failed to enable port forwarding.";
    //     client->close();
    //     sshWorker->deleteLater();
    //     return;
    // }

    // sshWorker->forward_data(client);
    // // Connect signals for data forwarding
    // // connect(sshWorker, &SSHWorker::on_response_ready, this, [=](QByteArray data) {
    // //     client->write(data);
    // // });

    // // connect(this, &SocksProxy::on_client_data, sshWorker, &SSHWorker::on_client_data);

    // // connect(client, &QTcpSocket::readyRead, this, [=]() {
    // //     QByteArray data = client->readAll();
    // //     emit on_client_data(data);
    // // });

    // connect(client, &QTcpSocket::disconnected, this, [=]() {
    //     qDebug() << "Client disconnected.";
    //     client->close();
    //     sshWorker->deleteLater();
    // });
}

void SocksProxy::open_connection() {
    this->listen(QHostAddress::Any, 1080);
}
