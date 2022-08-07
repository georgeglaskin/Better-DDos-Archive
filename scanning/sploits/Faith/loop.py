import time
import os

while 1:
    os.system("ulimit -n 999999; cat list.txt |  ./faith 8088")

    sleep(500)