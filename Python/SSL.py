import ssl, socket,os,threading,select,certifi,sys
import injector
proxyip="sg1.ssl.speedssh.com"
port=443

class PROXY:
    def proxy(self,host,port):
        try:
            s=socket.socket()
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            s.bind((host, port))
            s.listen(100)
            print('Server is running...')
            while True:
                client,address=s.accept()
                t=threading.Thread(target=self.CreateSSL,args=(client,address))
                t.start();
        except:
            pass
    def CreateSSL(self,client,address):
        data = client.recv(1024)
        ddata=data.decode('utf-8')
        host=ddata.split('\n')[0]
        method=host.split(' ')[0]
        if(method=='CONNECT'):
            host=host.split(' ')[1]
            port=host.split(':')[1]
            rport=port
            host=host.split(':')[0]
            try:
                address = socket.gethostbyname(host)
            except:
                address=host
            s = socket.socket()
            try:
                inj=injector.injector()
                s.connect((address, int(rport)))
                context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
                context.check_hostname = False
                context.verify_mode = ssl.CERT_NONE
                s=context.wrap_socket(s, server_hostname="free.facebook.com")
                inj.SendPayload(client,s,host,port)
                self.tunneling(client,s)
            except Exception as e:
                print(e)
            pass
    def tunneling(self,client,sockt):
        connected = True
        while connected == True:
            r, w, x = select.select([client,sockt], [], [client,sockt],3)
            if x: connected = False; break
            for i in r:
                try:
                    data = i.recv(2048)
                    print(data)
                    if not data: connected = False; break
                    if i is sockt:
                        client.send(data)
                    else:
                        sockt.send(data)
                except Exception as e:
                    connected = False;break
        client.close()
        sockt.close()
if __name__=='__main__':
    a=PROXY()
    a.proxy('127.0.0.1',8080)
