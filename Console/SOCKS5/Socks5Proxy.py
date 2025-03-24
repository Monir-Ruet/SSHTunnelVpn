import socket
import threading
import select
import codecs
SOCKS_VERSION=5
class proxy:
    def __init__(self):
        pass
    def handle(self,conn):
        version,method=conn.recv(2)
        methods=self.get_available_method(method,conn)
        conn.sendall(bytes([SOCKS_VERSION,0]))
        version,cmd,_,address_type=conn.recv(4)
        if address_type == 1:
            address = socket.inet_ntoa(conn.recv(4))
        elif address_type == 3:  # Domain name
            domain_length = conn.recv(1)[0]
            address = conn.recv(domain_length)
            address = socket.gethostbyname(address)
        port = int.from_bytes(conn.recv(2), 'big', signed=False)
        try:
            if cmd == 1:  # CONNET
                remote = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                remote.connect((address, port))
                bind_address = remote.getsockname()
                # print("* Connected to {} {}".format(address, port))
            else:
                conn.close()
            addr = int.from_bytes(socket.inet_aton(bind_address[0]), 'big', signed=False)
            port = bind_address[1]
            self.reply = b''.join([
                SOCKS_VERSION.to_bytes(1, 'big'),
                int(0).to_bytes(1, 'big'),
                int(0).to_bytes(1, 'big'),
                int(1).to_bytes(1, 'big'),
                addr.to_bytes(4, 'big'),
                port.to_bytes(2, 'big')
            ])
        except Exception as e:
            pass
        conn.sendall(self.reply)
        if self.reply[1] == 0 and cmd == 1:
            self.tunneling(conn, remote)
        conn.close()

    def tunneling(self,client,sockt):
        connected = True
        while connected == True:
            r, w, x = select.select([client,sockt], [], [client,sockt],3)
            if x: connected = False; break
            for i in r:
                try:
                    data = i.recv(1024)
                    if not data: connected = False; break
                    if i is sockt:
                        client.send(data)
                    else:
                        try:
                            print(data.decode())
                            print('\n')
                        except Exception as c:
                            pass
                        sockt.send(data)
                except Exception as e:
                    connected = False;break
        client.close()
        sockt.close()
    def exchange_loop(self, client, remote):
        while True:
            r, w, e = select.select([client, remote], [], [])
            if client in r:
                data=client.recv(2048)
                try:
                    a=data.decode('utf-8')
                    print(a)
                except Exception as e:
                    pass
                if remote.send(data) <= 0:
                    break

            if remote in r:
                data = remote.recv(2048)
                if client.send(data) <= 0:
                    break


    def get_available_method(self,method,conn):
        methods=[]
        for i in range(method):
            methods.append(ord(conn.recv(1)))
        return methods
    def run(self,host,port):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((host, port))
        s.listen()
        while True:
            conn,addr=s.accept()
            # print("* New Connection from {}".format(addr))
            t=threading.Thread(target=self.handle,args=(conn,))
            t.start()

if __name__=="__main__":
    proxy=proxy()
    proxy.run("127.0.0.1",1080)
