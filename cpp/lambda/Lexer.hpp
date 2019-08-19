#include <string>
#include <unordered_set>
#include <unordered_map>
#include <sstream>

template<class T>
using uset = std::unordered_set<T>;
using string  = std::string;

enum TokenType
{
    ID,
    BIND,
    EQUAL,
    OPEN,
    CLOSE,
    NL,
    EOL //end of line
};

struct Position
{
    string File;
    int Index;
    int Line;
    int Char;
};

struct Token 
{
    TokenType Type;
    Position Position;
    string Cargo = "";

    string Stringify()
    {
        static std::unordered_map<TokenType, string> map = 
        {
            {ID, "identifier"}, 
            {BIND, "binder"},
            {EQUAL, "equal"}, 
            {OPEN, "("}, {CLOSE, ")"},
            {NL, "newline"}, {EOL, "end of line"}
        };
        return map[Type] + (Cargo.size() > 0 ? ("("+Cargo+")") : "");
    }
};

const string     lowercase = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const uset<char> ID_SET    (lowercase.begin(), lowercase.end());
const uset<char> SPACE_SET {' ', '\t'};

class Lexer 
{
    protected:
        Token Current;
        int Line = 0, Char = 0;
        string File = "";
    private:
        string Text = "";
        int Index = 0;
        bool EOL = true;
    public:
        Token NextToken()
        {
            if(EOL) return Create(TokenType::EOL);
            char ch = GetChar();
            Next();
            if(ID_SET.count(ch) == 1) return Create(TokenType::ID, ch + Match(ID_SET));
            else if(SPACE_SET.count(ch) == 1)
            { Match(SPACE_SET); return NextToken(); }
            else switch(ch)
            {
                case('$'):
                case(','): return Create(TokenType::BIND);
                case('('): return Create(TokenType::OPEN);
                case(')'): return Create(TokenType::CLOSE);
                case(':'): 
                    if(!EOL && (GetChar() == '=')){Next(); return Create(TokenType::BIND);}
                    return Create(TokenType::EQUAL);
                case('='):
                    if(!EOL && (GetChar() == '>'))
                    { Next(); return Create(TokenType::BIND); }
                    return Create(TokenType::EQUAL);
                case('\n'):
                case(';'):
                    return Create(TokenType::NL);
                case('#'):
                    while(!EOL && (GetChar() != '\n')) Next();
                    if(!EOL) Next();
                    return NextToken();
                default: Error(ch); return Current;
            }
        }
        Token Create(TokenType type, string cargo){
            Token tok = {type, GetPosition(), cargo};
            Current = tok;
            return tok;
        }
        Token Create(TokenType type){
            Token tok = {type, GetPosition()};
            Current = tok;
            return tok;
        }
        void Error(char ch){
            std::ostringstream s;
            s << "Unexpected character '" << ch << "' at line " << Line << " char " << Char << " in file "<< File;
            throw s.str();
        }
        string Match(const uset<char> set){
            string matched = "";
            while(!EOL && (set.count(GetChar()) == 1)) matched += GetChar();
            return matched;
        }
        void PartialReset() { Index = 0; EOL  = Text.size() == Index; }
        void Reset() { PartialReset(); Line = 0; Char = 0;}
        void Read(string text){ Text = text; PartialReset();}

        char GetChar(){return Text[Index];}
        Position GetPosition(){
            Position pos = {File, Index, Line, Char};
            return pos;
        }
        void Next(){
            if(EOL) return;
            if(GetChar() == '\n') { Line ++; Char = -1; }
            if(Index == Text.size() - 1) EOL = true;
            Index++;
            Char++;
        }
};