from calc.runtime import *
import sys

arguments   = sys.argv[1:]
interactive = False

if "-i" in arguments:
    interactive = True
    arguments.remove("-i")

if len(arguments) > 1:
    print("Error: Too many command line arguments!")
    exit(1)

code = ""
if len(arguments):
    try:
        file = open(arguments[-1],"r")
        code = file.read()
        file.close()
    except Exception as err:
        print(err.msg)
        exit(1)

r = Runtime()
r.exec_block(code)

if interactive:
    cmd = ""
    while cmd != "exit":
        try:
            r.exec_block(cmd)
        except Exception as e:
            print(e)
        cmd = input(">>> ")



