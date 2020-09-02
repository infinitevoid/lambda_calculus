from calc.runtime import Runtime

r = Runtime()

c = "atom 1;true := x,y => x; false = x $ y $ y; or = x,y => x true y"
while c != "exit":
    r.exec_block(c)
    c = input(">>> ")


