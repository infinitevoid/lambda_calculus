from .parser import parse_line
from .lexer import Lexer
from .tree import *

def eval_expr(tree):
    if type(tree) is Bind:
        return Bind(tree.link, eval_expr(tree.expr))
    elif type(tree) is Atom:
        return tree
    elif type(tree) is Call:
        expr = eval_expr(tree.expr)
        arg = tree.arg
        if type(arg) is not Link:
            arg = eval_expr(arg)
        while type(expr) is Link and expr.cargo != None:
            expr = expr.cargo
        if type(expr) is Bind:
            return eval_expr(expr.dispatch(arg))
        else:
            return Call(expr, arg)
    else: #it's a link
        if tree.glob or tree.cargo == None:
            return tree
        else:
            return eval_expr(tree.cargo)

class Runtime:
    def __init__(self):
        self.scope = {}
        self.lexer = Lexer()
    def exec_block(self, block):
        self.lexer.reset()
        lines = block.replace(";","\n").split("\n")
        for l in lines:
            self.lexer.read(l)
            tree = parse_line(self.lexer, self.scope)
            self.exec_tree(tree)
    def exec_tree(self, tree):
        if type(tree) is DefineAtom:
            self.scope[tree.id] = Atom(tree.id)
        elif type(tree) is DefineFunc:
            self.scope[tree.id] = Link(tree.id, eval_expr(tree.expr), glob = True)
        elif tree != None:
            res = eval_expr(tree)
            print(res)