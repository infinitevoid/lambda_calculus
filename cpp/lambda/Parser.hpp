
#include "Lexer.hpp"
#include "Tree.hpp"
#include <unordered_map>
#include <iostream>


using Scope = std::unordered_map<string, Node*>;


struct Stmt 
{   
    string Id = "";
    Node* Expr = nullptr;
};

class Parser: public Lexer
{
    private:
        enum Primary {BIND, NODE, ID, NIL};
        Node* LastNode = nullptr;
        string LastString = "";

    public:
        channel Warnings;

        Primary MatchPrimary(Scope scope)
        {
            //cout << "Pirmary\n";
            Token tok = NextToken();
            switch(tok.Type)
            {
                case TokenType::BIND: {return Primary::BIND;}
                case TokenType::OPEN: {
                    //cout << "(\n";
                    Node* node = ParseExpr(nullptr, scope);
                    if(Current.Type != TokenType::CLOSE){
                        TokenError(Current);
                    }
                    //cout << ")\n";
                    LastNode = node;
                    return Primary::NODE;}
                case TokenType::ID: {
                    LastString = tok.Cargo;
                    return Primary::ID;
                    }
                case TokenType::NL:
                case TokenType::CLOSE:
                case TokenType::EOL:
                    LastNode = nullptr;
                    return Primary::NIL;
                default:
                    TokenError(tok);
                    return Primary::NIL;
            }   
        }
        Node* ParseExpr(Node* left, Scope scope)
        {
            //cout << "Node\n" << std::endl;
            Primary mode = MatchPrimary(scope);
            if(left == nullptr)
            {
                //cout << "left nullptr\n";
                if(mode == Primary::ID)
                    return ParseExpr(LastString, scope);
                else if(mode == Primary::NODE)
                    return LastNode;
                else TokenError(Current);
            }
            else
            {
                //cout << "switch\n";
                switch(mode){
                    case(Primary::NIL):
                        return left;
                    case(Primary::ID):
                        ExpectSymbol(LastString, scope);
                        return ParseExpr(new Call(left, scope[LastString]), scope);
                    case(Primary::BIND): TokenError(Current); return nullptr;
                    //Primary::NODE
                    default: return ParseExpr(new Call(left, LastNode), scope);
                }
            }
        }
        Node* ParseExpr(string left, Scope scope)
        {
            //cout << "String\n";
            Primary mode = MatchPrimary(scope);
            if(mode == Primary::BIND){
                //cout << "String->Bind\n";
                //TODO assert that last primary was string?!
                //TODO check if new symbol overwrites anything in the scope tabel
                // if so => warn the user...
                Link* link = new Link(left, false);
                Scope new_scope(scope.begin(), scope.end());
                CheckSymbol(left, scope);
                new_scope[left] = link;
                return new Bind(link, ParseExpr(nullptr, new_scope));
            }
            else if(mode == Primary::NIL){
                //cout << "String->NIL\n";
                ExpectSymbol(left, scope);
                return scope[left];
            }
            else{
                //cout << "String->ELSE\n";
                ExpectSymbol(left, scope);
                Node* node = scope[left];
                if(mode == Primary::ID){
                    ExpectSymbol(LastString, scope);
                    return ParseExpr(new Call(node, scope[LastString]), scope);
                }
                //mode == Primary::NODE
                return ParseExpr(new Call(node, LastNode), scope);
            }
        }
        Stmt ParseLine(Scope scope)
        {
            //std::cout << "Hello there!" << std::endl;
            Stmt stmt;
            Token tok = NextToken();
            if(tok.Type == TokenType::ID)
            {
                if(tok.Cargo == "atom"){
                    Token id = NextToken();
                    ExpectToken(id, TokenType::ID);
                    Token eol = NextToken();
                    ExpectToken(eol, TokenType::EOL);
                    stmt.Id = id.Cargo;
                    return stmt;
                }
                else{
                    Token op = NextToken();
                    Node* expr;
                    if(op.Type == TokenType::EQUAL){
                        expr = ParseExpr(nullptr, scope);
                        //NextToken();
                        Skip(TokenType::CLOSE);
                        ExpectToken(Current, TokenType::EOL);
                        stmt.Id = tok.Cargo;
                        stmt.Expr = expr;
                        return stmt;
                    }
                    PartialReset();
                    expr = ParseExpr(nullptr, scope);
                    //NextToken();
                    Skip(TokenType::CLOSE);
                    ExpectToken(Current, TokenType::EOL);
                    stmt.Expr = expr;
                    return stmt;
                }
            }
            else if(tok.Type != TokenType::EOL) TokenError(tok);
            return stmt;
        }
        void TokenError(Token tok)
        {
            channel stream;
            stream << "Unexpected token '" << tok.Stringify() << "' ";
            PrintPosition(stream);
            throw stream.str();
        }
        void ExpectSymbol(string sym, Scope scope){
            if(scope.count(sym) == 1) return;
            channel stream;
            stream << "Undefined symbol '" << sym << "' ";
            PrintPosition(stream); 
            throw stream.str();
        }
        void ExpectToken(Token tok, TokenType type){
            if(tok.Type == type) return;
            channel stream;
            Token dummy = {type};
            stream << "Expected '" << dummy.Stringify()  << "' but got'" << tok.Stringify() << "' ";
            PrintPosition(stream);
            throw stream.str();
        }
        void CheckSymbol(string sym, Scope scope)
        {
            if(scope.count(sym) > 0 ){
                Warnings << "Warning: " << sym << " overwrites local bound or global variabel ";
                PrintPosition(Warnings); Warnings << "\n";
            }
        }
        void Skip(TokenType skip)
        {
            if(Current.Type == skip) NextToken();
        }
};