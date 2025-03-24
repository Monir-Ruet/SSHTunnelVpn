#include "socket.h"

Socket::Socket()
{
    fd=socket(AF_INET,SOCK_STREAM,0);
}

Socket::Socket(int x,struct sockaddr_in a){
    fd=x;
    address=a;
}

int Socket::Bind(std::string host,int port){
    memset(&address,0,sizeof(address));
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_port=htons(port);
    return bind(fd,(struct sockaddr*)&address,sizeof(address));
}

int Socket::Listen(int backlog)
{
    return listen(fd,backlog);
}

Socket* Socket::Accept()
{
    struct sockaddr_in remote;
    memset(&remote,0,sizeof(remote));
    socklen_t clilen=sizeof(remote);
    int conn=accept(fd,(struct sockaddr*)&remote,&clilen);
    Socket* sock= new Socket(conn,remote);
    return sock;
}

void Socket::Setsockopt()
{
    int enable=1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,&enable, sizeof(enable));
}

int Socket::Send(std::string s)
{
    return send(fd,s.c_str(),s.size(),0);
}

std::string Socket::Read(int sz)
{
    char buffer[sz];
    std::string s;
    recv(fd,buffer,sizeof(buffer),0);
    for(int i=0;i<sz && buffer[i]!='\0';i++)
        s.push_back(buffer[i]);
    return s;
}

void Socket::Close()
{
    close(fd);
}

int Socket::Connect(std::string host, int port)
{
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if(inet_addr(host.c_str()) == -1){
        struct hostent *he;
        struct in_addr **addr_list;
        if ( (he = gethostbyname( host.c_str() ) ) == NULL){
            herror("Failed to resolve hostname");
            return -1;
        }
        addr_list = (struct in_addr **) he->h_addr_list;
        for(int i = 0; addr_list[i] != NULL; i++){
            address.sin_addr = *addr_list[i];
            break;
        }
    }else
        address.sin_addr.s_addr = inet_addr( host.c_str() );
    return connect(fd, (struct sockaddr *)&address , sizeof(address));
}
