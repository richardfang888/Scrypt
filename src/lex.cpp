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
    // for (int i = 0; i < argc; i++) {
    //     cout << argv[i] << endl;
    // }
    while (getline(cin, input))
    {
        text += input;
        if (!cin.eof())
        {
            text += '\n';
        }
    }

    // text = "(/49 50. \n 12 ($ 7..8 .3))";
    //text = "(+ aded 5 (/ 4 2))";
    tokens = readTokens(text);
    checkLexErrors(tokens);
    printTokens(tokens);

    return 0;
}
