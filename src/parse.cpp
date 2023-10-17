#include "lib/parse.hpp"
#include <iostream>

using namespace std;

AST::AST(const vector<Token> &tokens)
{
    int index = 0;
    root = buildTree(tokens, index, tokens.size() - 1);
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

Node *AST::buildTree(const vector<Token> &tokens, int &index, int eindex)
{
    if (index > eindex)
    {
        cerr << "Error: Invalid S expression. Unexpected end of input." << endl;
        return nullptr;
    }

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
            node->children.push_back(buildTree(tokens, index, eindex));
        }

        if (index < eindex && tokens[index].type == RIGHT_PAREN)
        {
            index++;
        }
        else
        {
            cerr << "Error: Invalid S expression. Missing or misplaced right parenthesis." << endl;
            deleteNode(node);
            return nullptr;
        }

        return node;
    }
    else
    {
        cerr << "Error: Invalid S expression. Unexpected token '" << token.text << "'" << endl;
        index++;
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
        cerr << "Error: Invalid AST node." << endl;
        return 0.0;
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
                    cerr << "Error: Division by zero." << endl;
                    return 0.0;
                }
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
    printInfix(root);
    cout << endl;
}
void AST::printInfix(const Node *node) const
{
    if (!node)
        return;

    if (node->token.type == FLOAT)
    {
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

    AST ast(tokens);
    ast.printInfix();
    cout << ast.evaluateAST() << endl;

    return 0;
}
