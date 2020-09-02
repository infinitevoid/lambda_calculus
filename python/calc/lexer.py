
alphabet = "abcdefghijklmnopqrstuvwxyz"
special  = "!?|+-_"
digits   = "0123456789"
ID_SET = set(alphabet + alphabet.upper()+special+digits)
SPACE_SET   = set(" \t")


class Token:
    ID    = 0
    BIND  = 1
    EQUAL = 2
    OPEN  = 3
    CLOSE = 4
    NL    = 5
    EOF   = 6 
    __slots__ = ("type", "cargo","pos")
    def __init__(self, type, pos, cargo = None):
        self.type  = type
        self.pos   = pos
        self.cargo = cargo
    def __str__(self):
        map = {0:"ID",1:"BIND",2:"EQUAL",3:"OPEN",4:"CLOSE",5:"NL",6:"EOF"}
        if self.cargo != None:
            return map[self.type]+":"+self.cargo+" "
        return map[self.type]+" "

class Lexer:
    def __init__(self, text = ""):
        self.text  = text
        self.reset()
    def reset(self):
        self.partial_reset()
        self.line,self.char  = 0,0
        self.eof   = len(self.text) == self.index
    def partial_reset(self):
        self.current = None
        self.index = 0
        self.eof   = len(self.text) == self.index
    def read(self, text):
        self.text = text
        self.partial_reset()
    def get(self):
        return self.text[self.index]
    def next(self):
        if not self.eof:
            if self.get() == "\n":
                self.line += 1
                self.char = -1
            if self.index == len(self.text)-1:
                self.eof = True
            self.index += 1
            self.char  += 1
    def match_set(self, set):
        word = ""
        while not self.eof and self.get() in set:
            word += self.get()
            self.next()
        return word
    def pos(self):
        return (self.line+1, self.char)
    def error(self,ch):
        #TODO        
        raise Exception("Unexpected character '%s' at line %d char %d."%(ch, *self.pos()))
    def token(self, type, cargo = None):
        tok =  Token(type, self.pos(), cargo)
        self.current = tok
        return tok
    def next_token(self):
        if not self.eof:
            ch = self.get()
            self.next()
            if ch in ID_SET:
                return self.token(Token.ID, ch+self.match_set(ID_SET))
            elif ch in SPACE_SET:
                self.match_set(SPACE_SET)
                return self.next_token()
            elif ch == "$" or ch == ",":
                return self.token(Token.BIND)
            elif ch == "(": 
                return self.token(Token.OPEN)
            elif ch == ")":
                return self.token(Token.CLOSE) 
            elif ch == ":":
                if not self.eof and self.get() == "=":
                    self.next()
                    return self.token(Token.EQUAL)
                else:
                    self.error(ch)
            elif ch == "=":
                if not self.eof and self.get() == ">":
                    self.next()
                    return self.token(Token.BIND)
                return self.token(Token.EQUAL)
            elif ch == "\n" or ch == ";":
                return self.token(Token.NL)
            elif ch == "#":
                while not self.eof and self.get() != "\n":
                    self.next()
                if not self.eof: self.next()
                return self.next_token()
            else:
                self.error(ch)
        else:
            return self.token(Token.EOF)

