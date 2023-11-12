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

    // lex and then check for lex errors
    tokens = readTokens(text);

    if (tokens.empty() || tokens.back().text == "error")
    {
        exit(1);
    }
    if (!tokens.empty())
    {
        printTokens(tokens);
    }

    return 0;
}
