/**
 * @author      : Monir (monir81.mk@gmail.com)
 * @created     : Wednesday Feb 16, 2022 11:03:14 +06
 */
#include<bits/stdc++.h>
#include<sys/socket.h>
#include<locale>
#include<arpa/inet.h>
#include "INIReader.h"
#include<netdb.h>
#include<unistd.h>
using namespace std;
const char nl='\n';

string payload;

string replace(string s, string t,string r) {
    size_t pos = 0;
    while((pos = s.find(t, pos)) != string::npos) {
      s.replace(pos, t.length(), r);
      pos += r.length();
    }
    return s;
}
string PayloadFormate(string payload,string host,string port){
  payload=replace(payload, "[crlf]", "\r\n");
  payload = replace(payload,"[crlf]","\r\n");
  payload = replace(payload,"[cr]","\r");
  payload = replace(payload,"[lf]","\n");
  payload = replace(payload,"[protocol]","HTTP/1.0");
  payload = replace(payload,"[ua]","Dalvik/2.1.0");
  payload = replace(payload,"[raw]","CONNECT "+host+":"+port+" HTTP/1.0\r\n\r\n");
  payload = replace(payload,"[real_raw]","CONNECT "+host+":"+port+" HTTP/1.0\r\n\r\n");
  payload = replace(payload,"[netData]","CONNECT "+host+":"+port +" HTTP/1.0");
  payload = replace(payload,"[realData]","CONNECT "+host+":"+port+" HTTP/1.0");
  payload = replace(payload,"[split_delay]","[delay_split]");
  payload = replace(payload,"[split_instant]","[instant_split]");
  payload = replace(payload,"[method]","CONNECT");
  payload = replace(payload,"mip","127.0.0.1");
  payload = replace(payload,"[ssh]",host+":"+port);
  payload = replace(payload,"[lfcr]","\n\r");
  payload = replace(payload,"[host_port]",host+':'+port);
  payload = replace(payload,"[host]",host);
  payload = replace(payload,"[port]",port);
  payload = replace(payload,"[auth]","");
  return payload;
}

class Socket{
  struct sockaddr_in address;
  public:
  int root;
  Socket(){
    root=socket(AF_INET,SOCK_STREAM,0);
  }
  Socket(int x,struct sockaddr_in a){
    root=x,address=a;
  }
  int Bind(string host,int port){
    memset(&address,0,sizeof(address));
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_port=htons(port);
    return bind(root,(struct sockaddr*)&address,sizeof(address));
  }
  int Listen(int backlog){
    return listen(root,backlog);
  }
  Socket Accept(){
    struct sockaddr_in remote;
    memset(&remote,0,sizeof(remote));
    socklen_t clilen=sizeof(remote);
    int conn=accept(root,(struct sockaddr*)&remote,&clilen);
    Socket a(conn,remote);
    return a;
  }
  void Setsockopt(){
    int enable=1;
    setsockopt(root, SOL_SOCKET, SO_REUSEADDR,&enable, sizeof(enable));
  }
  int Send(string s){
    return send(root,s.c_str(),s.size(),0);
  }
  string Read(int sz){
    char buffer[sz];
    string s;
    recv(root,buffer,sizeof(buffer),0);
    for(int i=0;i<sz && buffer[i]!='\0';i++)
      s.push_back(buffer[i]);
    s+='\0';
    return s;
  }
  void Close(){
    close(root);
  }
  int Connect(string host,int port){
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
    return connect(root , (struct sockaddr *)&address , sizeof(address));
  }

};
vector<int>Z(string s){
  int n=s.size();
  vector<int>z(n);
  for(int i=1,l=0,r=-1;i<n;i++){
    if(i<=r)
      z[i]=min(r-i+1,z[i-l]);
    while(i+z[i]<n && s[i+z[i]]==s[z[i]])
      z[i]++;
    if(i+z[i]-1>r)
      l=i,r=i+z[i]-1;
  }
  return z;
}
string split(string s,string p,int i){
  s=p+s;
  vector<int>z=Z(s);
  string t;
  vector<string>v;
  for(int j=p.size();j<s.size();j++){
    if(z[j]==p.size()){
      if(t.size())
        v.push_back(t);
      t="";
      j+=p.size()-1;
    }else
      t+=s[j];
  }
  if(t.size())
    v.push_back(t);
  if(i>v.size())
    return "";
  return v[i];
}
#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define BOLD "\x1B[1m"

void ASYNCIO(Socket client,Socket remote)
{
  const int SZ=10000;
  fd_set io;
  bool connected=1;
  char buffer[SZ];
  int cl=client.root,re=remote.root;
  vector<int>v{cl,re};
  for (;;){
    if(!connected) break;
    FD_ZERO(&io);
    FD_SET(cl, &io);
    FD_SET(re, &io);
    if (select(max(cl,re)+1, &io, NULL, NULL, NULL) < 0){
      perror("Tunnel: select()");
      break;
    }
    for(auto i:v){
      if(FD_ISSET(i,&io)){
        try{
          int count=recv(i,buffer,sizeof(buffer),0);
          if(i==cl){
            cout<<BOLD<<KGRN<<"[++++ Request ++++]\n"<<nl;
            send(re,buffer,count,0);
          }
          else {
            cout<<BOLD<<KBLU<<"[++++ Response ++++]\n"<<nl;
            send(cl,buffer,count,0);
          }
          cout<<KWHT<<buffer<<nl<<nl;
          if(count<=0){
            cout<<"[-] Disconnected\n";
            connected=0;break;
          }
        }
        catch(const exception & e){
          cerr<<e.what()<<nl;
          break;
        }
      }
    }
  }
  client.Close();
  remote.Close();
  return;
}
void handle(Socket conn){
  string packet=conn.Read(1024);
  string head=split(packet,"\n",0);
  string method=split(head," ",0);
  string url=split(head," ",1);
  string host=split(url,":",0);
  string p=split(url,":",1);
  if(method=="CONNECT"){
    int port=stoi(p);
    Socket remote;
    if(remote.Connect(host,port)<0) return;
    conn.Send("HTTP/1.1 200 Connection established\r\n\r\n");
    remote.Send(PayloadFormate(payload,host,p));
    ASYNCIO(conn,remote);
  }
}
void Server(string host,int port){
  Socket s;
  s.Setsockopt();
  if(s.Bind(host,port)<0){
    perror("[-] Bind Failed");
    return;
  }
  if(s.Listen(100)<0){
    perror("[-] Listen Failed\n");
    return;
  }
  cout<<"Listening on "<<host<<":"<<port<<nl;
  while(1){
    Socket conn=s.Accept();
    thread t(handle,conn);
    t.join();
  }
}
bool multi=1;

int main(){
  INIReader reader("settings.ini");
  payload=reader.Get("config","payload","UNKNOWN");
  Server("127.0.0.1",8080);
}
