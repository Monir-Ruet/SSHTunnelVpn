import socket
import re
import configparser


class injector:
    def __init__(self):
        pass
    def conf(self):
        config = configparser.ConfigParser()
        try:
            config.read_file(open('settings.ini'))
        except Exception as e:
            Print(R,'Settings.ini not found\n')
        return config

    def getpayload(self,config):
        payload = config['config']['payload']
        return payload

    def proxy(self,config):
        proxyhost = config['config']['proxyip']
        proxyport = int(config['config']['proxyport'])
        return [proxyhost,proxyport]
    def conn_mode(self,config):
        mode = config['mode']['connection_mode']
        return mode

    def auto_rep(self,config):
        result = config['config']['auto_replace']
        return result
    def payloadformating(self,payload,host,port):
        payload = payload.replace('[crlf]','\r\n')
        payload = payload.replace('[crlf*2]','\r\n\r\n')
        payload = payload.replace('[cr]','\r')
        payload = payload.replace('[lf]','\n')
        payload = payload.replace('[protocol]','HTTP/1.0')
        payload = payload.replace('[ua]','Dalvik/2.1.0')
        payload = payload.replace('[raw]','CONNECT '+host+':'+port+' HTTP/1.0\r\n\r\n')
        payload = payload.replace('[real_raw]','CONNECT '+host+':'+port+' HTTP/1.0\r\n\r\n')
        payload = payload.replace('[netData]','CONNECT '+host+':'+port +' HTTP/1.0')
        payload = payload.replace('[realData]','CONNECT '+host+':'+port+' HTTP/1.0')
        payload = payload.replace('[split_delay]','[delay_split]')
        payload = payload.replace('[split_instant]','[instant_split]')
        payload = payload.replace('[method]','CONNECT')
        payload = payload.replace('mip','127.0.0.1')
        payload = payload.replace('[ssh]',host+':'+port)
        payload = payload.replace('[lfcr]','\n\r')
        payload = payload.replace('[host_port]',host+':'+port)
        payload = payload.replace('[host]',host)
        payload = payload.replace('[port]',port)
        payload = payload.replace('[auth]','')
        return payload
    def SendPayload(self,conn,s,host,port):
        conn.send(b'HTTP/1.1 200 Connection established\r\n\r\n')
        payload = self.payloadformating(self.getpayload(self.conf()),host,port)
        print(payload)
        if '[split]' in payload or '[instant_split]' in payload or '[delay_split]' in payload:
            payload = payload.replace('[split]'        ,'||1.0||')
            payload = payload.replace('[delay_split]'  ,'||1.5||')
            payload = payload.replace('[instant_split]','||0.0||')
            req = payload.split('||')
            for payl in req:
                if payl in ['1.0','1.5','0.0'] :
                    delay = payl
                    time.sleep(float(delay))
                else:
                    s.send(payl.encode())

        elif '[repeat_split]' in payload  :
            payload = payload.replace('[repeat_split]','||1||')
            payload = payload.replace('[x-split]','||1||')
            req = payload.split('||')
            payl = []
            for element in req:
                if element and element == '1' :pass
                else:payl.append(element)
            rpspli = payl[0]+payl[0]
            s.send(rpspli.encode())
            s.send(payl[1].encode())
        elif '[reverse_split]' in payload or '[x-split]' in payload:
            payload = payload.replace('[reverse_split]','||2|')
            payload = payload.replace('[x-split]','||2|')
            req = payload.split('||')
            payl = []
            for element in req:
                if element and element == '2':pass
                else:payl.append(element)
            rvsplit = payl[0]+payl[1]
            s.send(rvsplit.encode())
            s.send(payl[1].encode())
        elif '[split-x]' in payload:
                payload = payload.replace('[split-x]','||3||')
                req = payload.split('||')
                xsplit = []
                for element in req:
                    if element and element == '3':pass
                    else:xsplit.append(element)
                    alpay = xsplit[0]+xsplit[1]
                    s.send(alpay.encode())
                time.sleep(1.0)
                s.send(xsplit[1].encode())
        else:
            print(payload)
            s.send(payload.encode())
