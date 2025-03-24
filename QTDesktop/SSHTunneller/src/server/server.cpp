#include "server.h"
#include <thread>

Server::Server(QObject *parent)
    : QObject{parent}
{
    socket = new Socket();
}

void Server::setupServer()
{
    std::string host = "localhost";
    int port = 1080;
    socket->Setsockopt();
    if(socket->Bind(host,port)<0){
        perror("[-] Bind Failed");
        return;
    }
    if(socket->Listen(100)<0){
        perror("[-] Listen Failed\n");
        return;
    }
    std::cout<<"Listening on "<<host<<":"<<port<<std::endl;
    while(1){
        Socket *remote=socket->Accept();
        std::thread t(&Server::socks5Handler, this, remote);
        t.detach();
    }
}

void Server::socks5Handler(Socket *remote){
    std::string data = remote->Read(2); // Read the client greeting
    if (data.empty() || static_cast<uint8_t>(data[0]) != 0x05) {
        qDebug() << "Invalid SOCKS5 version or empty data.";
        delete remote;
        return;
    }
    qDebug() << data;
    uint8_t numMethods = static_cast<uint8_t>(data[1]);
    data = remote->Read(numMethods); // Read the supported authentication methods
    qDebug() << data;
    // Respond with "no authentication required" (0x00)
    QByteArray response;
    response.append(0x05); // SOCKS5 version
    response.append(static_cast<char>(0x00)); // No authentication
    remote->Send(response.toStdString());

    // Step 2: Handle client request
    data = remote->Read(4); // Read the request header
    qDebug() << data;
    if (data.size() < 3 || static_cast<uint8_t>(data[0]) != 0x05 || static_cast<uint8_t>(data[1]) != 0x01) {
        qDebug() << "Invalid SOCKS5 request.";
        delete remote;
        return;
    }

    uint8_t addressType = static_cast<uint8_t>(data[3]);
    std::string destinationAddress;
    uint16_t destinationPort;
    qDebug() << data[3];

    if (addressType == 0x01) { // IPv4 address
        data = remote->Read(4); // Read the IPv4 address
        destinationAddress = std::to_string(static_cast<uint8_t>(data[0])) + "." +
                             std::to_string(static_cast<uint8_t>(data[1])) + "." +
                             std::to_string(static_cast<uint8_t>(data[2])) + "." +
                             std::to_string(static_cast<uint8_t>(data[3]));
    } else if (addressType == 0x03) { // Domain name
        uint8_t domainLength = static_cast<uint8_t>(remote->Read(1)[0]);
        destinationAddress = remote->Read(domainLength); // Read the domain name
    } else {
        qDebug() << "Unsupported address type.";
        delete remote;
        return;
    }

    data = remote->Read(2);
    destinationPort = (static_cast<uint8_t>(data[0]) << 8) | static_cast<uint8_t>(data[1]);

    // Step 4: Send success response to the client
    response.clear();
    response.append(0x05); // SOCKS5 version
    response.append(static_cast<char>(0x00));  // Success
    response.append(static_cast<char>(0x00));  // Reserved
    response.append(0x01); // IPv4 address type
    response.append(static_cast<char>(0x00));  // Placeholder for IPv4 address (0.0.0.0)
    response.append(static_cast<char>(0x00));
    response.append(static_cast<char>(0x00));
    response.append(static_cast<char>(0x00));
    response.append(static_cast<char>((destinationPort >> 8) & 0xFF)); // Port (high byte)
    response.append(static_cast<char>(destinationPort & 0xFF));        // Port (low byte)
    remote->Send(response.toStdString());


    qDebug() << destinationAddress << " : " <<destinationPort;
}
