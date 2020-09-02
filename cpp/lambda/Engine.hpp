#include "Parser.hpp"
#include <queue>
#include <iostream>

Node* EvalTree(Node* tree)
{
    if(dynamic_cast<Bind*>(tree))
    {
        Bind* bind = dynamic_cast<Bind*>(tree);
        return new Bind(bind->Placeholder, EvalTree(bind->Expr));
    }
    else if(dynamic_cast<Call*>(tree))
    {
        Call* call = dynamic_cast<Call*>(tree);
        Node* expr = EvalTree(call->Expr);
        Node* arg  = call->Arg;
        if(!dynamic_cast<Link*>(arg)) arg = EvalTree(arg);
        while(dynamic_cast<Link*>(expr))
        {
            Link* link = dynamic_cast<Link*>(expr);
            if(link->Fill == nullptr) break;
            expr = link->Fill;
        }
        if(dynamic_cast<Bind*>(expr)) 
            return EvalTree(dynamic_cast<Bind*>(expr)->Dispatch(arg));
        return new Call(expr, arg);
    }
    else if(dynamic_cast<Atom*>(tree)) return tree;
    else //it's a link
    {
        Link* link = dynamic_cast<Link*>(tree);
        if(link->Global || (link->Fill == nullptr))
            return tree;
        return EvalTree(link->Fill);
    }
}

class Engine
{
    Scope Globals;
    std::stringstream Stream;
    std::queue<string> Lines;
    string Line;

    public:
        Parser Parser;
        channel Output;

    void ExecBlock(string code)
    {
        Stream << code;
        Parser.Reset();
        while(std::getline(Stream, Line, '\n'))
            Lines.push(Line);
        while(!Lines.empty())
        {
            Parser.PartialReset();
            Line = Lines.front();
            Output << Line << ":\t\t\t";
            Lines.pop();
            Parser.Read(Line);
            Stmt stmt = Parser.ParseLine(Globals);
            ExecStmt(stmt);
        }
    }
    void ExecStmt(Stmt stmt)
    {
        if(stmt.Expr == nullptr && stmt.Id != "")
        {Define(stmt.Id, new Atom(stmt.Id)); Output << "new atom\n";}
        else if(stmt.Expr != nullptr && stmt.Id != "")
        {/*stmt.Expr->Debug(Output,0);*/ Link* link = new Link(stmt.Id, true); link->Fill = stmt.Expr; Output << "Defined: ";stmt.Expr->Print(Output); Output << "\n"; Define(stmt.Id,link);}
        else if(stmt.Expr == nullptr) return;
        else
        {
            Node* res = EvalTree(stmt.Expr);
            res->Print(Output);
            Output << "\n";
            //res->Debug(Output, 0);
        }
    }
    void Define(string key, Node* expr)
    {
        if(Globals.count(key) > 0)
        {
            channel s;
            s << "Global variable " << key << " can't be overwritten.";
            throw s.str();
        }
        Globals[key] = expr;
        //std::cout << "defined " << key << std::endl;
    }
};