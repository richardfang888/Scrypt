#include "lib/parse.hpp"
#include <iostream>

using namespace std;

AST::AST(const vector<Token> &tokens)
{
    if (tokens.empty())
    {
        printErrorTwo(Token{END, "", 0, 1, 1});
        return;
    }
    int index = 0;
    root = makeTree(tokens, index, tokens.size() - 1);
    if (index != static_cast<int>(tokens.size()) - 1)
    {
        deleteNode(root);
        printErrorTwo(tokens[index]);
        return;
    }
}

AST::~AST()
{
    deleteNode(root);
}

void AST::deleteNode(Node *node)
{
    if (node)
    {
        for (Node *child : node->children)
        {
            deleteNode(child);
        }
        delete node;
    }
}

Node *AST::makeTree(const vector<Token> &tokens, int &index, int eindex)
{
    Token token = tokens[index];
    if (token.type == FLOAT)
    {
        Node *node = new Node();
        node->token = token;
        index++;
        return node;
    }
    else if (token.type == LEFT_PAREN)
    {
        Node *node = new Node();
        index++;
        node->token = tokens[index++];

        while (index < eindex && tokens[index].type != RIGHT_PAREN)
        {
            node->children.push_back(makeTree(tokens, index, eindex));
        }

        if (index < eindex && tokens[index].type == RIGHT_PAREN)
        {
            index++;
        }
        else
        {
            printErrorTwo(tokens[index]);
            return nullptr;
        }

        return node;
    }
    else
    {
        printErrorTwo(token);
        return nullptr;
    }
}

double AST::evaluateAST()
{
    if (!root)
    {
        return 0;
    }
    return evaluate(root);
}

double AST::evaluate(Node *node) const
{
    if (node->token.type == FLOAT)
    {
        return stod(node->token.text);
    }
    else if (node->children.size() == 0)
    {
        printErrorTwo(node->token);
        return 2;
    }
    else
    {
        double result = evaluate(node->children[0]);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            Token opToken = node->token;
            if (opToken.type == PLUS)
            {
                result += evaluate(node->children[i]);
            }
            else if (opToken.type == MINUS)
            {
                result -= evaluate(node->children[i]);
            }
            else if (opToken.type == TIMES)
            {
                result *= evaluate(node->children[i]);
            }
            else if (opToken.type == DIVIDES)
            {
                double denominator = evaluate(node->children[i]);
                if (denominator != 0)
                {
                    result /= denominator;
                }
                else
                {
                    cerr << "Runtime error: division by zero." << endl;
                    exit(3);
                }
            }
            else
            {
                printErrorTwo(opToken);
                return 2;
            }
        }
        return result;
    }
}

Node *AST::getRoot() const
{
    return root;
}

void AST::printInfix() const
{
    if (root && root->token.type != FLOAT)
        cout << "(";

    printInfix(root);

    if (root && root->token.type != FLOAT)
        cout << ")";

    cout << endl;
}
void AST::printInfix(const Node *node) const
{
    if (!node)
        return;

    if (node->token.type == FLOAT)
    {
        double val = stod(node->token.text);
        if (val == static_cast<int>(val))
            cout << static_cast<int>(val);
        else
            cout << node->token.text;
    }
    else
    {
        bool isFirst = true;
        for (const auto &child : node->children)
        {
            if (!isFirst)
            {
                cout << " " << node->token.text << " ";
            }
            else
            {
                isFirst = false;
            }
            if (child->token.type != FLOAT)
            {
                cout << "(";
            }
            printInfix(child);
            if (child->token.type != FLOAT)
            {
                cout << ")";
            }
        }
    }
}

void printErrorTwo(const Token &token)
{
    cout << "Unexpected token at line " << token.lineNumber
         << " column " << token.columnNumber << ": "
         << token.text << endl;
    exit(2);
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

    // text = "";

    tokens = readTokens(text);
    checkLexErrors(tokens);

    AST ast(tokens);
    ast.printInfix();
    cout << ast.evaluateAST() << endl;

    return 0;
}
