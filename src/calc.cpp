#include "lib/calc.hpp"
#include <iostream>
#include <unordered_map>
#include <stack>
#include <limits>
#include <cmath>

AST::AST(const vector<Token> &tokens)
{
    if (tokens.empty())
    {
        printErrorTwo(Token{END, "", 0, 1, 1});
        return;
    }
    int index = 0;
    root = makeTree(tokens, index);
    checkTree(root, 0, 0, OTHER);

    // CAUSING ERROR (figure out later)
    // if (index != static_cast<int>(tokens.size()) - 1)
    // {
    //     deleteNode(root);
    //     printErrorTwo(tokens[index]);
    //     return;
    // }
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

Node *AST::makeNode(const Token &token)
{
    Node *node = new Node();
    node->token = token;
    return node;
}

// Recursively creates an AST from a list of tokens,
// checks for if there are formatting errors on converted S expression
Node *AST::makeTree(const vector<Token> &tokens, int &index)
{
    return parseInfix(tokens, index);
}

// Function to parse and build an AST from an infix expression
Node *AST::parseInfix(const vector<Token> &tokens, int &index)
{
    return parseAssignment(tokens, index);
}

// Function to parse assignment expressions
Node *AST::parseAssignment(const vector<Token> &tokens, int &index)
{
    Node *left = parseAddition(tokens, index);
    if (match(tokens, index, TokenType::ASSIGN))
    {
        Node *assignNode = makeNode(tokens[index]);
        Node *right = parseAssignment(tokens, ++index);
        assignNode->children.push_back(left);
        assignNode->children.push_back(right);
        return assignNode;
    }
    return left;
}

// Function to parse addition and subtraction expressions
Node *AST::parseAddition(const vector<Token> &tokens, int &index)
{
    Node *left = parseMultiplication(tokens, index);
    while (match(tokens, index, TokenType::PLUS) || match(tokens, index, TokenType::MINUS))
    {
        Token opToken = tokens[index++];
        Node *right = parseMultiplication(tokens, index);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse multiplication and division expressions
Node *AST::parseMultiplication(const vector<Token> &tokens, int &index)
{
    Node *left = parsePrimary(tokens, index);
    while (match(tokens, index, TokenType::TIMES) || match(tokens, index, TokenType::DIVIDES))
    {
        Token opToken = tokens[index++];
        Node *right = parsePrimary(tokens, index);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse primary expressions
Node *AST::parsePrimary(const vector<Token> &tokens, int &index)
{
    Token token = tokens[index++];
    if (token.type == TokenType::FLOAT || token.type == TokenType::IDENTIFIER)
    {
        return makeNode(token);
    }
    else if (token.type == TokenType::LEFT_PAREN)
    {
        Node *expression = parseAssignment(tokens, index);
        if (!match(tokens, index, TokenType::RIGHT_PAREN))
        {
            // Handle missing closing parenthesis error
            // Implement error handling here
            printErrorTwo(token);
            return nullptr;
        }
        ++index; // Increment index to skip the closing parenthesis
        return expression;
    }
    else
    {
        // Handle unexpected token error
        // Implement error handling here
        printErrorTwo(token);
        return nullptr;
    }
}

// Utility function to check if the current token matches the expected token type
bool AST::match(const vector<Token> &tokens, int index, TokenType expectedType)
{
    if (index >= int(tokens.size()))
    {
        return false;
    }
    return tokens[index].type == expectedType;
}

void AST::checkTree(Node *node, int childNum, int totalChildren, TokenType OPERATOR) const
{
    if (OPERATOR == ASSIGN)
    {
        if (childNum != totalChildren - 1 && node->token.type != IDENTIFIER)
        {
            printErrorTwo(node->token);
        }
    }
    long unsigned int i = 0;
    while (i < node->children.size())
    {
        checkTree(node->children[i], i, node->children.size(), node->token.type);
        i++;
    }
}

double AST::evaluateAST(unordered_map<string, double> &variables)
{
    if (!root)
    {
        return 0;
    }
    return evaluate(root, variables);
}

// Evaluates the given AST node and returns the result of the original expression.
double AST::evaluate(Node *node, unordered_map<string, double> &variables) const
{
    if (!node)
    {
        cout << "Error: node is null" << endl; // temp solution
        return 0;
    }
    // If the node holds a FLOAT token, simply return its value.
    if (node->token.type == FLOAT)
    {
        return stod(node->token.text);
    }
    // If the node is an IDENTIFIER token, return its value if it exists in the variables map
    // NOTE: This only runs if an IDENTIFIER is found not during assignment
    if (node->token.type == IDENTIFIER)
    {
        string identifierText = node->token.text;

        // Check if the identifier exists in the variables unordered_map
        auto iter = variables.find(identifierText);
        if (iter != variables.end())
        {
            // Return the value of the identifier
            return iter->second;
        }
        else
        {
            // Handle error: Unknown identifier
            cout << "Runtime error: unknown identifier " + identifierText << endl;
            return numeric_limits<double>::quiet_NaN();
        }
    }
    // Node is an operator but has no children
    else if (node->children.size() == 0)
    {
        printErrorTwo(node->token);
        return 2;
    }
    // Node is assignment operator
    else if (node->token.type == ASSIGN)
    {
        double result = evaluate(node->children[node->children.size()-1], variables);
        for (size_t i = node->children.size() - 2; i >= 0; i--)
        {
            if (node->children[i]->token.type != IDENTIFIER)
            {
                // invalid assignment error
                printErrorTwo(node->token);
                return 2;
            }
            variables[node->children[i]->token.text] = result;
        }
    }
    // Node is a non-assignment operator
    else
    {
        // Iterate over the rest of the children to apply the operation.
        double result = evaluate(node->children[0], variables);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            Token opToken = node->token;
            if (opToken.type == PLUS)
            {
                result += evaluate(node->children[i], variables);
            }
            else if (opToken.type == MINUS)
            {
                result -= evaluate(node->children[i], variables);
            }
            else if (opToken.type == TIMES)
            {
                result *= evaluate(node->children[i], variables);
            }
            else if (opToken.type == DIVIDES)
            {
                // Check for division by zero.
                double denominator = evaluate(node->children[i], variables);
                if (denominator != 0)
                {
                    result /= denominator;
                }
                else
                {
                    cout << "Runtime error: division by zero." << endl;
                    return numeric_limits<double>::quiet_NaN();
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

// Prints an output 2 error message for a given token
void printErrorTwo(const Token &token)
{
    cout << "Unexpected token at line " << token.lineNumber
         << " column " << token.columnNumber << ": "
         << token.text << endl;
}

int main(int argc, const char **argv)
{
    string input;
    string text;
    unordered_map<string, double> variables;

    while (getline(cin, input)) // Keep reading until EOF
    {
        // text = "x = y = 5";
        vector<Token> tokens = readTokens(input);
        checkLexErrors(tokens);

        AST ast(tokens);
        ast.printInfix();
        double result = ast.evaluateAST(variables);

        if (!isnan(result))
        {
            cout << result << endl;
        }
    }

    return 0;
}