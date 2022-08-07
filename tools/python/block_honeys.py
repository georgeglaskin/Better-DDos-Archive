from http.client import ImproperConnectionState
import sys
import os

joined = []

blocked = []
blocks = 0

def is_blocked(host):
    for block in blocked:
        if block == host:
            return 1
    return 0

def is_joined(host):
    for join in joined:
        if join == host:
            return 1
    return 0

def split_joined(line):
    args = line.split(":")

    joined.append(args[0])


def find_joined():
    os.system("netstat -an | grep :" + sys.argv[1] + " | grep ESTABLISHED | sort -u | awk '{ print $4; }' > joined.txt")

    with open("joined.txt", "r") as f:
        for line in f.readlines():
            split_joined(line)
    
    os.remove("joined.txt")

def block(host):
    global blocks

    if is_blocked(host) == 1 or is_joined(host):
        return

    #print(host + " is now blocked")
    cmd = "iptables -I INPUT -s " + host + " -j DROP"

    os.system(cmd)

    blocked.append(host)

    blocks += 1

def check(line):
    count = 0
    args = line.split(" ")

    if args[8] != "200":
        block(args[0])

def filter():
    with open("/var/log/apache2/access.log", "r") as f:
        for line in f.readlines():
            check(line)

def main():
    if len(sys.argv) != 2:
        print(sys.argv[0] + " botport")
        return
    
    find_joined()

    print("loaded: " + str(len(joined)) + " ips")

    filter()

    print("blocked: " + str(blocks) + " hosts")

main()
            