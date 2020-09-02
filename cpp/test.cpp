#include "lambda/Engine.hpp"
#include <iostream>
#include <fstream>
#include <streambuf>

using namespace std;

int main(){
    Engine engine; 
    std::ifstream t("./test.lc");
    string code((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
    
    //code = "true = a,b => a\n#asfdjasfkdljasl\nfalse = x,y $ (y)";
    cout << "found file: " << (code.size() > 0) << endl;
    try
    {
        engine.ExecBlock(code);
        cout << engine.Output.str() << engine.Parser.Warnings.str() << endl;
        engine.Output.str("");
        engine.Parser.Warnings.str("");

    }
    catch(string s) {std::cout << s << std::endl;}
}