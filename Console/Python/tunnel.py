import socket
import threading
import select
import codecs
import injector
import configparser
import re
import time

class proxy:
    def __init__(self):
        self.squidip='150.95.178.151'
        self.squidip_port='8888'
    def handle(self,conn):
        data = conn.recv(1024)
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
            # address=self.squidip
            # rport=self.squidip_port
            remote = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                remote.connect((address, int(rport)))
                inj=injector.injector()
                # print("[+] Connected to",address,rport,'\n')
                inj.SendPayload(conn,remote,host,port)
                self.tunneling(conn,remote)
            except:
                print("[-] Connection Failed...\n")
        else :
            pass

    def tunneling(self,client,remote):
        connected = True
        while connected == True:
            r, w, x = select.select([client,remote], [], [client,remote],3)
            if x: connected = False; break
            for i in r:
                try:
                    data = i.recv(10000)
                    if not data: connected = False; break
                    print(data,'\n')
                    if i is remote:
                        client.send(data)
                    else:
                        remote.send(data)
                except Exception as e:
                    connected=False;break
        client.close()
        remote.close()
        print("[-] Disconnected ")

    def run(self,host,port):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((host, port))
        s.listen()
        print('[+] Server running on ',host,port,'\n')
        while True:
            conn,addr=s.accept()
            t=threading.Thread(target=self.handle,args=(conn,))
            t.start()
if __name__=="__main__":
    proxy=proxy()
    proxy.run("127.0.0.1",8080)

