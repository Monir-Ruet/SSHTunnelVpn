#include "sshworker.h"

#define BUFFER_SIZE 65536

SSHWorker::SSHWorker(QObject *parent)
    : QObject{parent},ssh(SSH::instance()), channel(nullptr) {
    ssh_set_log_level(SSH_LOG_PROTOCOL);
}

SSHWorker::~SSHWorker(){
    cleanup();
}

void SSHWorker::cleanup() {
    qDebug() << "Cleaning up resources...";

    if (channel) {
        if (ssh_channel_is_open(channel)) {
            int rc = ssh_channel_send_eof(channel);
            if (rc == SSH_ERROR) {
                qDebug() << "Error sending EOF to channel:" << ssh_get_error(ssh->getSession());
            }
            ssh_channel_free(channel);
        }
    }

    qDebug() << "Cleanup complete.";
}

bool SSHWorker::enablePortForwarding(QByteArray address, int32_t port) {
    int rc;

    // Check if SSH session is connected
    if (!ssh_is_connected(ssh->getSession())) {
        qDebug() << "SSH session is not connected.";
        return false;
    }

    // Allocate new channel
    channel = ssh_channel_new(ssh->getSession());
    if (!channel) {
        qDebug() << "Failed to allocate SSH channel.";
        return false;
    }
    rc = ssh_channel_open_forward(channel, address, port, "localhost", 0);
    if (rc == SSH_AGAIN) {
        qDebug() << "Retrying channel open forward...";
        return false; // Retry later
    } else if (rc != SSH_OK) {
        qDebug() << "Port forwarding request:"
                 << "Address:" << address
                 << "Port:" << port
                 << "Destination: localhost:" + std::to_string(0);

        qDebug() << "Error opening SSH port forwarding:" << ssh_get_error(ssh->getSession()) << "rc=" << rc;
        cleanup();
        return false;
    }

    return true;
}

void SSHWorker::forward_data(QTcpSocket* socket) {
    ssh_session session = ssh_channel_get_session(channel);
    char buffer[BUFFER_SIZE];

    while (true) {
        // Check if the client socket has data to read
        if (socket->bytesAvailable() > 0) {
            // Read data from the client socket
            qint64 nbytes = socket->read(buffer, BUFFER_SIZE);
            if (nbytes <= 0) {
                qDebug() << "Client disconnected.";
                break;
            }

            // Forward data to the SSH channel
            int total_written = 0;
            while (total_written < nbytes) {
                int nwritten = ssh_channel_write(channel, buffer + total_written, nbytes - total_written);
                if (nwritten <= 0) {
                    qDebug() << "Error writing to SSH channel.";
                    break;
                }
                total_written += nwritten;
            }
        }

        // Check if the SSH channel has data to read
        int nbytes = ssh_channel_read_nonblocking(channel, buffer, BUFFER_SIZE, 0);
        if (nbytes > 0) {
            qint64 total_written = 0;
            while (total_written < nbytes) {
                qint64 nwritten = socket->write(buffer + total_written, nbytes - total_written);
                if (nwritten <= 0) {
                    qDebug() << "Error writing to client socket.";
                    break;
                }
                total_written += nwritten;
            }
            socket->flush();
        } else if (nbytes == SSH_ERROR) {
            qDebug() << "Error reading from SSH channel.";
            break;
        }

        // Wait for activity on either the client socket or the SSH channel
        if (!socket->waitForReadyRead(5)) { // 1-second timeout
            if (socket->state() != QTcpSocket::ConnectedState) {
                qDebug() << "Client disconnected.";
                break;
            }
        }
    }

    qDebug() << "Data forwarding stopped.";
    cleanup();
}
