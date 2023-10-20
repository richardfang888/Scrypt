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

// Recursively creates an AST from a list of tokens,
// checks for if there are formatting errors on converted S expression
Node *AST::makeTree(const vector<Token> &tokens, int &index, int eindex)
{
    Token token = tokens[index];

    // Base case: if the token is a FLOAT, simply return a new node with that token.
    if (token.type == FLOAT)
    {
        Node *node = new Node();
        node->token = token;
        index++;
        return node;
    }
    // If the token is a LEFT_PAREN, then a new operation is starting.
    else if (token.type == LEFT_PAREN)
    {
        Node *node = new Node();
        index++;

        // Check if the token after LEFT_PAREN is a valid operation. If not, throw an error.
        if (index > eindex || (tokens[index].type != PLUS && tokens[index].type != MINUS &&
                               tokens[index].type != TIMES && tokens[index].type != DIVIDES))
        {
            printErrorTwo(tokens[index]);
            deleteNode(node);
            return nullptr;
        }
        node->token = tokens[index++];

        // Error checking for an empty operation
        if (tokens[index].type == RIGHT_PAREN)
        {
            printErrorTwo(tokens[index]);
            deleteNode(node);
            return nullptr;
        }

        // While there are more tokens inside the parentheses, recursively create child nodes.
        while (index < eindex && tokens[index].type != RIGHT_PAREN)
        {
            node->children.push_back(makeTree(tokens, index, eindex));
        }

        // Error handling for incorrect expressions (like "(+ 1 2" without the closing parenthesis)
        if (node->children.empty() || (node->children.size() == 1 && node->children[0]->token.type != FLOAT))
        {
            printErrorTwo(tokens[index - 1]);
            deleteNode(node);
            return nullptr;
        }
        else if (index < eindex && tokens[index].type == RIGHT_PAREN)
        {
            index++;
        }
        // If the token is neither a FLOAT nor a LEFT_PAREN, it's unexpected.
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

// Evaluates the given AST node and returns the result of the original expression.
double AST::evaluate(Node *node) const
{
    // If the node holds a FLOAT token, simply return its value.
    if (node->token.type == FLOAT)
    {
        return stod(node->token.text);
    }

    // If the node does not have any children, throw an error.
    else if (node->children.size() == 0)
    {
        printErrorTwo(node->token);
        return 2;
    }
    else
    {
        // Iterate over the rest of the children to apply the operation.
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
                // Check for division by zero.
                double denominator = evaluate(node->children[i]);
                if (denominator != 0)
                {
                    result /= denominator;
                }
                else
                {
                    cout << "Runtime error: division by zero." << endl;
                    exit(3);
                }
            }
            else
            {
                // If the operation is unrecognized, print an error message.
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

// Prints an output 2 error message for a given token
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

    tokens = readTokens(text);
    checkLexErrors(tokens);

    AST ast(tokens);
    ast.printInfix();
    cout << ast.evaluateAST() << endl;

    return 0;
}