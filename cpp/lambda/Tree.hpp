#include <string>
#include <sstream>

using string   = std::string;
using channel  = std::ostringstream;


// utility function for debugging
void tabs(channel &str, int ind)
{
    for(int i = 0; i < ind; i++) str << '\t';
}

struct Node
{
    virtual Node* Copy(){ return new Node();};
    virtual void Debug(channel &stream, int ind){
        tabs(stream, ind);
        stream << "NODE\n";
    }
    virtual void Print(channel &stream){}
};

struct Link: public Node
{

    bool Global;
    Node* Fill = nullptr;
    const string Name;
    Link(string name, bool glob): Name(name), Global(glob){}
    void Set(Node * val){Fill = val;}
    Node* Copy()
    {
        if(Global || (Fill == nullptr)) return this;
        // TODO?!
        return Fill->Copy();
    }
    void Debug(channel &stream, int ind)
    {
        tabs(stream, ind);
        stream << "LINK " << Name << "\n";
        if(Fill != nullptr) Fill->Debug(stream, ind+1);
    }
    void Print(channel &stream){stream << Name;}
};

struct Bind: public Node
{
    Link* Placeholder;
    Node* Expr;
    int Copies = 0;
    Bind(Link* link, Node* expr): Placeholder(link), Expr(expr){}
    Node* Dispatch(Node* val)
    {
        Placeholder->Set(val);
        Node* temp = Expr->Copy();
        return temp;
    }
    Node* Copy()
    {
        Copies++;
        Link* nl = new Link(Placeholder->Name + "'" + std::to_string(Copies), false);
        Placeholder->Set(nl);
        Bind* nb = new Bind(nl, Expr->Copy());
        Placeholder->Set(nullptr);
        return nb;
    }
    void Debug(channel &stream, int ind)
    {
        Placeholder->Debug(stream, ind+1);
        tabs(stream, ind);
        stream << "BIND\n";
        Expr->Debug(stream, ind+1);
    }
    void Print(channel &stream)
    {
        stream << "(";
        Placeholder->Print(stream);
        stream << " => ";
        Expr->Print(stream);
        stream << ")";    
    }
};

struct Atom: public Node
{
    string Name = "";
    Atom(string name): Name(name){}
    Node* Copy(){return this;}
    void Debug(channel &str, int ind){
        tabs(str, ind);
        str << "ATOM " << Name << "\n";
    }
    void Print(channel &stream){ stream << Name;}
};

struct Call: public Node
{

    Node* Expr;
    Node* Arg;
    
    Call(Node* expr, Node* arg): Expr(expr), Arg(arg){}
    Node* Copy(){return new Call(Expr->Copy(), Arg->Copy());}
    void Debug(channel &stream, int ind)
    {
        Expr->Debug(stream, ind+1);
        tabs(stream, ind);
        stream << "CALL\n";
        Arg->Debug(stream, ind+1);
    }
    void Print(channel &stream)
    {
        stream << "(";
        Expr->Print(stream);
        stream << ")";
        stream << " ";
        stream << "(";
        Arg->Print(stream);
        stream << ")";    
    }
};