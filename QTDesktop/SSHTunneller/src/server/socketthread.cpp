#include "socketthread.h"
#include "sshworker.h"

SocketThread::SocketThread(qintptr descriptor, QObject *parent):
    m_descriptor(descriptor), m_socket(nullptr)
{}

SocketThread::~SocketThread()
{
    m_socket->close();
    delete m_socket;
    qDebug() << "Cleaning up";
}

void SocketThread::run()
{
    m_socket = new QTcpSocket;
    m_socket->setSocketDescriptor(m_descriptor);
    qDebug() << "New m_socket Connected";

    if (!m_socket->waitForReadyRead(5000)) { // Timeout after 5 seconds
        qDebug() << "Timeout waiting for data.";
        return;
    }
    QByteArray data = m_socket->read(2);
    if (data.size() < 2 || static_cast<uint8_t>(data[0]) != 0x05) {
        qDebug() << "Invalid SOCKS5 version or incomplete greeting.";
        return;
    }

    uint8_t numMethods = static_cast<uint8_t>(data[1]);
    if (m_socket->bytesAvailable() < numMethods) {
        return; // Wait for more data
    }

    QByteArray methods = m_socket->read(numMethods);
    bool noAuthSupported = methods.contains(0x00);

    if (!noAuthSupported) {
        qDebug() << "No supported authentication methods.";
        QByteArray response;
        response.append(0x05); // SOCKS5 version
        response.append(0xFF); // No acceptable methods
        m_socket->write(response);
        return;
    }

    // Respond with "no authentication required" (0x00)
    QByteArray response;
    response.append(0x05); // SOCKS5 version
    response.append(static_cast<char>(0x00)); // No authentication
    m_socket->write(response);

    m_socket->waitForReadyRead();

    data = m_socket->read(4);
    if (data.size() < 4 || static_cast<uint8_t>(data[0]) != 0x05 || static_cast<uint8_t>(data[1]) != 0x01) {
        qDebug() << "Invalid SOCKS5 request.";
        return;
    }

    uint8_t addressType = static_cast<uint8_t>(data[3]);
    QByteArray address;
    quint16 port;

    if (addressType == 0x01) { // IPv4 address
        if (m_socket->bytesAvailable() < 4 + 2) {
            return; // Wait for more data
        }
        address = m_socket->read(4);
        port = (static_cast<quint8>(m_socket->read(1)[0]) << 8) + static_cast<quint8>(m_socket->read(1)[0]);
    } else if (addressType == 0x03) { // Domain name
        if (m_socket->bytesAvailable() < 1) {
            return; // Wait for more data
        }
        uint8_t domainLength = static_cast<uint8_t>(m_socket->read(1)[0]);
        if (m_socket->bytesAvailable() < domainLength + 2) {
            return; // Wait for more data
        }
        address = m_socket->read(domainLength);
        port = (static_cast<quint8>(m_socket->read(1)[0]) << 8) + static_cast<quint8>(m_socket->read(1)[0]);
    } else {
        qDebug() << "Unsupported address type:" << addressType;
        return;
    }

    // Send success response
    QByteArray successResponse;
    successResponse.append(char(5)); // SOCKS5 version
    successResponse.append(char(0)); // Success
    successResponse.append(char(0)); // Reserved
    successResponse.append(char(1)); // IPv4 address type
    successResponse.append(char(0)); // Placeholder for IPv4 address (0.0.0.0)
    successResponse.append(char(0));
    successResponse.append(char(0));
    successResponse.append(char(0));
    successResponse.append(char((port >> 8) & 0xFF)); // High byte
    successResponse.append(char(port & 0xFF));        // Low byte
    m_socket->write(successResponse);

    // Create SSHWorker for port forwarding
    SSHWorker *sshWorker = new SSHWorker();
    if (!sshWorker->enablePortForwarding(address, port)) {
        qDebug() << "Failed to enable port forwarding.";
        quit();
        sshWorker->deleteLater();
        return;
    }

    connect(m_socket, &QTcpSocket::disconnected, this, [=]() {
        qDebug() << "m_socket disconnected.";
        sshWorker->deleteLater();
        quit();
    });

    sshWorker->forward_data(m_socket);
}
