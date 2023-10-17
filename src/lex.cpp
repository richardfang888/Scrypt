#include "lib/lex.hpp"
#include <iomanip>
#include <iostream>
#include <algorithm>

using namespace std;

int main(int argc, const char **argv)
{
    string input;
    string text;
    vector<Token> tokens;

    while (getline(cin, input))
    {
        text += input;
        if (!cin.eof())
        {
            text += '\n';
        }
    }

    // text = "(+(-2 4.444 ) 32(* 5 13.45))";
    tokens = readTokens(text);
    checkLexErrors(tokens);
    printTokens(tokens);

    return 0;
}
