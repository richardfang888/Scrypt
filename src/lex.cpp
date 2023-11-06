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

    tokens = readTokens(text);

    if (tokens.empty() || tokens.back().text == "error")
    {
        exit(1);
    }
    if (!tokens.empty())
    {
        printTokens(tokens);
    }

    // try 
    // {
    //     tokens = readTokens(text);
    //     printTokens(tokens);
    // }
    // catch(const lexer_error &e)
    // {
    //     cout << e.what() << endl;
    //     exit(1);
    // }

    return 0;
}
