#include "lib/format.hpp"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

using namespace std;


void AST::printAll() const
{
    if (dynamic_cast<WhileNode *>(node)) {
        // Node is a WhileNode
        printWhile();
    } else if (dynamic_cast<IfElseNode *>(node)) {
        // Node is an IfElseNode
        printIfElse();
    } else if (dynamic_cast<PrintNode *>(node)) {
        // Node is a PrintNode
        printPrint();
    } else {
        // Node is a normal Node
        printInfix();
    }
}

void AST::printInfix() const
{
    if (root && (root->token.type != FLOAT && root->token.type != IDENTIFIER && root->token.type != BOOLEAN))
        cout << "(";

    printInfix(root);
    if (root && (root->token.type != FLOAT && root->token.type != IDENTIFIER && root->token.type != BOOLEAN))
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
    else if (node->token.type == IDENTIFIER || node->token.type == BOOLEAN)
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
            if (child->token.type != FLOAT && child->token.type != IDENTIFIER && child->token.type != BOOLEAN)
            {
                cout << "(";
            }
            printInfix(child);
            if (child->token.type != FLOAT && child->token.type != IDENTIFIER && child->token.type != BOOLEAN)
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

    // while (getline(cin, input))
    // {
    //     text += input;
    //     if (!cin.eof())
    //     {
    //         text += '\n';
    //     }
    // }

    //test cases:
    text = "x = 42";

    // lex
    tokens = readTokens(text);

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
        tree.printAll();
    }

    return 0;
}