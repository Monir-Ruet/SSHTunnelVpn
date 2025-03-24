#ifndef SOCKET_H
#define SOCKET_H

#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>

#include <QObject>

class Socket
{
public:
    explicit Socket();
    Socket(int x,struct sockaddr_in a);

    int Bind(std::string host,int port);
    int Listen(int backlog);
    Socket* Accept();
    void Setsockopt();
    int Send(std::string s);
    std::string Read(int sz);
    void Close();
    int Connect(std::string host,int port);

private:
    struct sockaddr_in address;
    int fd;
};

#endif // SOCKET_H
