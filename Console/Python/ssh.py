import subprocess
import socket
import threading
import random
import time
import sys,os,re
import configparser

# colors
bg=''
G = bg+'\033[32m'
O = bg+'\033[33m'
GR = bg+'\033[37m'
R = bg+'\033[31m'

class sshRunn:
    def __init__(self,inject_host,inject_port):
        self.inject_host = inject_host
        self.inject_port = inject_port

    def ssh_client(self,socks5_port,host,port,user,password):
        try:
            dynamic_port_forwarding = '-CND {}'.format(socks5_port)
            host = host
            port = port
            username = user
            password=password
            inject_host= self.inject_host
            inject_port= self.inject_port
            nc_proxies_mode = [f'corkscrew {inject_host} {inject_port} %h %p', f'nc -X CONNECT -x {inject_host}:{inject_port} %h:%p']
            # arg = str(sys.argv[1])
            # if arg == '1':
            nc_proxy = nc_proxies_mode[0]
            # else:
                # nc_proxy = nc_proxies_mode[1]
            response = subprocess.Popen((
                        f'sshpass -p {password} ssh -o "ProxyCommand={nc_proxy}" {username}@{host} -p {port} -v {dynamic_port_forwarding} ' + '-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null '),
                    shell=True,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT)
            for line in response.stdout:
                line = line.decode('utf-8',errors='ignore').lstrip(r'(debug1|Warning):').strip() + '\r'
                print(line)
                if 'send disconnect' in line or 'authentication code incorrect' in line:
                    self.ssh_client(socks5_port,host,port,user,password)
        except KeyboardInterrupt:
            sys.exit('stoping ..')
    def create_connection(self,host,port,user,password):
        global soc , payload
        try:
            regx = r'[a-zA-Z0-9_]'
            if re.match(regx,host):
                try:
                    ip = socket.gethostbyname(host)
                except:
                    ip = host
            thread=threading.Thread(target=self.ssh_client,args=('1080',ip,port,user,password))
            thread.start()
        except ConnectionRefusedError:
            soc.close()
        except KeyboardInterrupt:
            sys.exit()

if __name__=='__main__':
    start = sshRunn('127.0.0.1','8080')
    config = configparser.ConfigParser()
    try:
        config.read_file(open('settings.ini'))
    except Exception as e:
        sys.exit()

    host = config['ssh']['host']
    port = config['ssh']['port']
    user = config['ssh']['username']
    password = config['ssh']['password']
    if host:
        start.create_connection(host,port,user,password)
    else:
        sys.exit


