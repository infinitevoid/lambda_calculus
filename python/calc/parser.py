from .tree import *
from .lexer import *


def token_error(lexer, tok):
    #TODO
    raise Exception("Unexpected token '%s' at line %d char %d"%(str(tok), *lexer.pos()))

def check_symbol(lexer, sym, table):
    if sym in table: return
    raise Exception("Undefined symbol '%s' at line %d char %d"%(str(sym), *lexer.pos()))

def check_token(lexer, tok, type):
    if tok.type == type: return
    raise Exception("Unexpected token '%s' at line %d char %d"%(str(tok), *lexer.pos()))

def match_primary(lexer, links):
    tok = lexer.next_token()
    if tok.type == Token.BIND:
        return Token.BIND
    elif tok.type == Token.OPEN:
        node =  parse_expr(lexer, None, links)
        if lexer.current.type != Token.CLOSE:
            token_error(lexer, lexer.current)
        return node
    elif tok.type == Token.ID:
        return tok.cargo
    elif tok.type in {Token.NL, Token.CLOSE, Token.EOF}:
        return None
    else:
        token_error(lexer, tok)


def parse_expr(lexer, left, links):
    if left == None:
        left = match_primary(lexer, links)
        if type(left) is not str:
            token_error(lexer, lexer.current)
        return parse_expr(lexer, left, links)
    else:
        tok = match_primary(lexer, links)
        if tok == Token.BIND:
            l = Link(left)
            new_links = links.copy()
            new_links[left] = l
            return Bind(l, parse_expr(lexer, None, new_links))
        elif tok == None:
            if type(left) is str:
                check_symbol(lexer, left, links)
                left = links[left]
            return left
        else:
            if type(left) is str:
                check_symbol(lexer, left, links)
                left = links[left]
            if type(tok) is str:
                check_symbol(lexer, tok, links)
                return parse_expr(lexer, Call(left, links[tok]), links)
            else:
                return parse_expr(lexer, Call(left, tok), links)

def parse_line(lexer, links):
    tok = lexer.next_token()
    if tok.type == Token.ID:
        if tok.cargo == "atom":
            id = lexer.next_token()
            check_token(lexer, id, Token.ID)
            eof = lexer.next_token()
            check_token(lexer, eof, Token.EOF)
            return DefineAtom(id.cargo)
        else:
            op = lexer.next_token()
            if op.type == Token.EQUAL:
                expr = parse_expr(lexer, None, links)
                check_token(lexer, lexer.current, Token.EOF)
                return DefineFunc(tok.cargo, expr)
            else:
                lexer.partial_reset()
                expr = parse_expr(lexer, None, links)
                check_token(lexer, lexer.current, Token.EOF)
                return expr
    elif tok.type == Token.EOF:
        return None
    else:
        token_error(lexer, tok)
