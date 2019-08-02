# lambda_calculus
Lambda-Calculus interpreter in python and cpp.

## python
The python implementation can be used via `python/main.py [-i] example.lc`.
The -i option opens a console after running the script allowing you interact with the runtime.
##Syntax
```
# a comment...
# identity function
x => x
# a simple definition
first := x, y => x
# another representation
first = x $ y $ x
```
You might notice that `$`,`=>` and `,` all essentially do the same thing;
they bind the left variable to the right expression.
##Example
```
# true and false can be implemented as functions that decide between two elements
true  := x,y => x
false := x,y => y
or    := x,y => x true y
# and, xor and neg are implemented in python/lib.lc
# you can also implement the natural numbers as functions
zero := x,y   => y
succ := x,y,z => y (x y z)
# now some numbers in peano style
one := succ zero
two := succ one
# essentailly a number n as a function of y z means apply y n-times on z
# hence, we can implement addition
add := x,y => x succ y
```
For more information check [this pdf](https://www.inf.fu-berlin.de/lehre/WS03/alpi/lambda.pdf) out.
