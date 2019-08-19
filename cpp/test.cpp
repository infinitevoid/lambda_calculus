#include "lambda/Parser.hpp"
#include <iostream>

int main(){
    Parser parser;
    parser.Read("a $ (b, b, d => a)");

    Token tok;
    do{ tok = parser.NextToken(); std::cout << tok.Stringify() << std::endl;}
    while (tok.Type != EOL);

    parser.Reset();

    Scope scope;
    try{
        Node* tree = parser.ParseExpr(nullptr, scope);
        channel a; 
        tree->Debug(a, 0);
        std::cout << a.str() << std::endl;
    }
    catch(string s) {std::cout << s << std::endl;}
}