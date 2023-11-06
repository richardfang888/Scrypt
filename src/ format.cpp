#include "lib/scrypt.hpp"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

using namespace std;

void AST::printInfix() const
{
    if (root && (root->token.type != FLOAT && root->token.type != IDENTIFIER))
        cout << "(";

    printInfix(root);
    if (root && (root->token.type != FLOAT && root->token.type != IDENTIFIER))
        cout << ")";

    cout << endl;
}

// Prints the infix notation of a given AST.
void AST::printInfix(const Node *node) const
{
    if (!node)
    {
        return;
    }
    else if (node->token.type == FLOAT)
    {
        double val = stod(node->token.text);
        if (val == static_cast<int>(val))
            cout << static_cast<int>(val);
        else
            cout << node->token.text;
    }
    else if (node->token.type == IDENTIFIER)
    {
        cout << node->token.text;
    }
    else
    {
        bool isFirst = true;
        for (const auto &child : node->children)
        {
            if (!child)
                continue;
            if (!isFirst)
            {
                cout << " " << node->token.text << " ";
            }
            else
            {
                isFirst = false;
            }
            if (child->token.type != FLOAT && child->token.type != IDENTIFIER)
            {
                cout << "(";
            }
            printInfix(child);
            if (child->token.type != FLOAT && child->token.type != IDENTIFIER)
            {
                cout << ")";
            }
        }
    }
}


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

    //test cases:
    //text = "(= b c (+ 6 3 4))";
    //text = "(= a b (+ 6 3 4)) \n (+ 1 a 3)";
    //text ="(= a 3)";
    //text = "(+ 4 5 7)";
    //text = "(= a b 3 z)";
    //text = "(=(+x)89)";
    //text = "(= x (+ 3 3) 4) ";

    // lex and then check for lex errors
    tokens = readTokens(text);
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
    // tokens = readTokens(text);

    // set up variables for muti expression parsing
    int index = 0;
    vector<AST> trees;
    std::unordered_map<std::string, double> variables;
    if (tokens.back().text == "error")
    {
        exit(1);
    }

    // parse the tokens and put into trees
    while(tokens[index].type != END)
    {
        AST ast(tokens, index);
        trees.push_back(ast);
    }
    //print and evaluate trees
    for(auto tree : trees)
    {
        tree.printInfix();
    }

    return 0;
}