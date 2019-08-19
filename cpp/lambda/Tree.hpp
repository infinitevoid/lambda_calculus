#include <string>
#include <sstream>

using string = std::string;
using channel  = std::ostringstream;


// utility function for debugging
void tabs(channel &str, int ind)
{
    for(int i = 0; i < ind; i++) str << '\t';
}

class Node
{
    public:
        virtual Node* Copy(){ return new Node();};
        virtual void Debug(channel &stream, int ind){
            tabs(stream, ind);
            stream << "NODE\n";
        };
};

class Link: public Node
{
    private:
        bool Global;
        Node* Fill = nullptr;
    public:
        const string Name;
        Link(string name, bool glob): Name(name), Global(glob){}
        void Set(Node * val){Fill = val;}
        Node* Copy()
        {
            if(Global || (Fill == nullptr)) return this;
            // TODO?!
            return Fill->Copy();
        }
        void Debug(channel &str, int ind)
        {
            tabs(str, ind);
            str << "LINK " << Name << "\n";
            if(Fill != nullptr) Fill->Debug(str, ind+1);
        }
};

class Bind: public Node
{
    private:
        Link* Placeholder;
        Node* Expr;
        //int Copies = 0;
    public:
        Bind(Link* link, Node* expr): Placeholder(link), Expr(expr){}
        Node* Dispatch(Node* val)
        {
            Placeholder->Set(val);
            Node* temp = Expr->Copy();
            return temp;
        }
        Node* Copy()
        {
            //Copies++;
            Link* nl = new Link(Placeholder->Name, false);
            Placeholder->Set(nl);
            Bind* nb = new Bind(nl, Expr->Copy());
            Placeholder->Set(nullptr);
            return nb;
        }
        void Debug(channel &str, int ind)
        {
            Placeholder->Debug(str, ind+1);
            tabs(str, ind);
            str << "BIND\n";
            Expr->Debug(str, ind+1);
        }
};

class Atom: public Node
{
    private:
        string Name;
    public:
        Node* Copy(){return this;}
        void Debug(channel &str, int ind){
            tabs(str, ind);
            str << "ATOM " << Name << "\n";
        }
};

class Call: public Node
{
    private:
        Node* Expr;
        Node* Arg;
    public:
        Call(Node* expr, Node* arg): Expr(expr), Arg(arg){}
        Node* Copy(){return new Call(Expr->Copy(), Arg->Copy());}
        void Debug(channel &str, int ind)
        {
            Expr->Debug(str, ind+1);
            tabs(str, ind);
            str << "CALL\n";
            Arg->Debug(str, ind+1);
        }
};