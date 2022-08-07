import sys

with open(sys.argv[1], "r") as f:
    for line in f.readlines():
        args = line.split(":")

        with open(sys.argv[2], "a") as fp:
            fp.write(args[0] + "\n")

            