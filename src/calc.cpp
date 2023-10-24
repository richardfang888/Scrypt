#include "lib/calc.hpp"
#include <iostream>
#include <unordered_map>
#include <stack>

AST::AST(const vector<Token> &tokens)
{
    if (tokens.empty())
    {
        printErrorTwo(Token{END, "", 0, 1, 1});
        return;
    }
    int index = 0;
    root = makeTree(tokens, index, tokens.size() - 1);
    checkTree(root, 0, 0, OTHER);
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

int AST::getPrecedence(string op) const {
    if (op == "+" || op == "-") {
        return 1;
    }
    else if (op == "*" || op == "/") {
        return 2;
    }
    else if (op == "^") {
        return 3;
    }
    return 0;
}

Node *AST::makeNode(const Token &token)
{
    Node *node = new Node();
    node->token = token;
    return node;
}

// Recursively creates an AST from a list of tokens,
// checks for if there are formatting errors on converted S expression
Node *AST::makeTree(const vector<Token> &tokens, int &index, int eindex)
{
    stack<Node *> nodeStack;
    stack<Token> tokenStack;

    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].type == LEFT_PAREN)
        {
            tokenStack.push(tokens[i]);
        }
        else if (tokens[i].type == FLOAT || tokens[i].type == IDENTIFIER)
        {
            Node *node = makeNode(tokens[index]);
            nodeStack.push(node);
        }
        else if (getPrecedence(tokens[i].text) > 0)
        {
            while (!tokenStack.empty() && tokenStack.top().type != LEFT_PAREN && 
                   ((tokens[i].text != "^" && getPrecedence(tokens[i].text) <= getPrecedence(tokenStack.top().text)) || 
                   (tokens[i].text == "^" && getPrecedence(tokens[i].text) < getPrecedence(tokenStack.top().text))))
            {
                Node *node = makeNode(tokenStack.top());
                tokenStack.pop();

                Node *node1 = nodeStack.top();
                nodeStack.pop();

                Node *node2 = nodeStack.top();
                nodeStack.pop();

                node->children.push_back(node2);
                node->children.push_back(node1);
                nodeStack.push(node);
            }
            tokenStack.push(tokens[i]);
        }
        else if (tokens[i].type == RIGHT_PAREN) {
            while (!tokenStack.empty() && tokenStack.top().type != LEFT_PAREN) {
                Node *node = makeNode(tokenStack.top());
                tokenStack.pop();

                Node *node1 = nodeStack.top();
                nodeStack.pop();

                Node *node2 = nodeStack.top();
                nodeStack.pop();

                node->children.push_back(node2);
                node->children.push_back(node1);
                nodeStack.push(node);
            }
            tokenStack.pop();
        }
    }
    return nodeStack.top();
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
            else if (opToken.type == ASSIGN)
            {
                result = evaluate(node->children[i]);
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

    // while (getline(cin, input))
    // {
    //     text += input;
    //     if (!cin.eof())
    //     {
    //         text += '\n';
    //     }
    // }

    text = "x = y = 0 + 1 + 2 * 3 - 4 / (5 + 6)";

    tokens = readTokens(text);
    checkLexErrors(tokens);

    AST ast(tokens);
    ast.printInfix();
    cout << ast.evaluateAST() << endl;

    return 0;
}