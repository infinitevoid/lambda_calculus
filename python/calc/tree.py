
class Node: pass
class Link(Node):
    def __init__(self, name, cargo = None, glob = False):
        self.name  = name
        self.cargo = cargo
        self.glob  = glob 
    def set(self, value):
        self.cargo = value
    def copy(self):
        if self.glob or self.cargo == None:
            return self
        #TODO is that allowed?!
        #actually doesn't have to return cargo right?!?!?
        #links only have to be resolved when called...
        return self.cargo.copy()
    def debug(self, ind = 0):
        txt = ind*"\t"+"LINK "+self.name+"\n"
        txt += "" if self.cargo == None else self.cargo.debug(ind+1)
        return txt
    def __str__(self):
        if self.glob or self.cargo == None:
            return self.name
        return str(self.cargo)

class Bind(Node): 
    def __init__(self, link, expr):
        self.link = link
        self.expr = expr
        self.copies = 0
    def dispatch(self, value):
        self.link.set(value)
        temp =  self.expr.copy()
        self.link.set(None)
        return temp
    def copy(self):
        self.copies += 1
        l = Link(self.link.name+"_"+str(self.copies))
        self.link.set(l)
        temp = Bind(l, self.expr.copy())
        self.link.set(None)
        return temp
    def debug(self, ind = 0):
        txt = self.link.debug(ind+1)
        txt += ind*"\t"+ "BIND\n"
        txt += self.expr.debug(ind+1)
        return txt
    def __str__(self):
        return "( %s $ %s )"%(str(self.link),str(self.expr))

class Atom(Node):
    def __init__(self,name):
        self.name = name
    def debug(self, ind = 0):
        return ind*"\t" + "ATOM "+ self.name +"\n"
    def copy(self):
        return self
    def __str__(self):
        return self.name
        
class Call(Node): 
    def __init__(self, expr, arg):
        self.expr = expr
        self.arg  = arg
    def copy(self):
        return Call(self.expr.copy(), self.arg.copy())
    def debug(self, ind = 0):
        txt = self.expr.debug(ind+1)
        txt += "\t"*ind + "CALL\n" 
        txt += self.arg.debug(ind+1)
        return txt
    def __str__(self):
        return "(%s) (%s)"%(str(self.expr), str(self.arg))

class DefineAtom(Node):
    def __init__(self, id):
        self.id = id
    def debug(self,ind=0):
        return ind*"\t"+"DEFINE ATOM %s"%self.id

class DefineFunc(Node):
    def __init__(self, id, expr):
        self.id   = id
        self.expr = expr
    def debug(self,ind=0):
        txt =  ind*"\t"+"DEFINE %s\n"%self.id
        txt += self.expr.debug(ind+1)
        return txt
