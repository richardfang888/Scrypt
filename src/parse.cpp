#include "lib/parse.hpp"
#include "lib/token.hpp"
#include "lib/lex.hpp"
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <memory>

using namespace std;

class Node
{
public:
    virtual ~Node() = default;
};

class OpNode : public Node
{
public:
    TokenType op;
    vector<unique_ptr<Node>> children;

    OpNode(TokenType op) : op(op) {}
};

class NumNode : public Node
{
public:
    double value;

    NumNode(double value) : value(value) {}
};

// Parser
class Parser
{
public:
    unique_ptr<Node> parse(const vector<Token> &tokens);

private:
    const vector<Token> *tokens;
    size_t current = 0;

    unique_ptr<Node> expression();
    unique_ptr<Node> term();
    unique_ptr<Node> factor();
    bool match(const vector<TokenType> &types);
    const Token &advance();
    const Token &peek();
    const Token &previous();
    bool isAtEnd();
    void error(const Token &token, const string &message);
};

unique_ptr<Node> Parser::parse(const vector<Token> &tokens)
{
    this->tokens = &tokens;
    return expression();
}

unique_ptr<Node> Parser::expression()
{
    unique_ptr<Node> node = term();

    while (match({PLUS, MINUS}))
    {
        Token oper = previous();
        unique_ptr<Node> right = term();
        unique_ptr<OpNode> opNode = make_unique<OpNode>(oper.type);
        opNode->children.push_back(move(node));
        opNode->children.push_back(move(right));
        node = move(opNode);
    }

    return node;
}

unique_ptr<Node> Parser::term()
{
    unique_ptr<Node> node = factor();

    while (match({TIMES, DIVIDES}))
    {
        Token oper = previous();
        unique_ptr<Node> right = factor();
        unique_ptr<OpNode> opNode = make_unique<OpNode>(oper.type);
        opNode->children.push_back(move(node));
        opNode->children.push_back(move(right));
        node = move(opNode);
    }

    return node;
}

unique_ptr<Node> Parser::factor()
{
    if (match({FLOAT}))
    {
        return make_unique<NumNode>(stod(previous().text));
    }
    if (match({LEFT_PAREN}))
    {
        unique_ptr<Node> node = expression();
        if (!match({RIGHT_PAREN}))
        {
            error(peek(), "Expect ')' after expression.");
        }
        return node;
    }
    error(peek(), "Expect expression.");
    return nullptr; // This will never be hit, but added to avoid compiler warning.
}

bool Parser::match(const vector<TokenType> &types)
{
    for (const auto &type : types)
    {
        if (isAtEnd())
            return false;
        if (peek().type == type)
        {
            advance();
            return true;
        }
    }
    return false;
}

const Token &Parser::advance()
{
    if (!isAtEnd())
        current++;
    return previous();
}

const Token &Parser::peek()
{
    return (*tokens)[current];
}

const Token &Parser::previous()
{
    return (*tokens)[current - 1];
}

bool Parser::isAtEnd()
{
    return current >= tokens->size() || peek().type == END;
}

void Parser::error(const Token &token, const string &message)
{
    cerr << "Unexpected token at line " << token.lineNumber << " column " << token.columnNumber << ": " << token.text << endl;
    exit(2); // Exit with error code 2 as instructed
}

// Print in infix form
void printInfix(Node *node)
{
    if (OpNode *op = dynamic_cast<OpNode *>(node))
    {
        cout << "(";
        for (size_t i = 0; i < op->children.size(); ++i)
        {
            printInfix(op->children[i].get());
            if (i < op->children.size() - 1)
            {
                switch (op->op)
                {
                case PLUS:
                    cout << " + ";
                    break;
                case MINUS:
                    cout << " - ";
                    break;
                case TIMES:
                    cout << " * ";
                    break;
                case DIVIDES:
                    cout << " / ";
                    break;
                default:
                    break; // Error case, or not an operator.
                }
            }
        }
        cout << ")";
    }
    else if (NumNode *num = dynamic_cast<NumNode *>(node))
    {
        cout << num->value;
    }
}

double evaluate(Node *node)
{
    if (OpNode *op = dynamic_cast<OpNode *>(node))
    {
        double result = evaluate(op->children[0].get());
        for (size_t i = 1; i < op->children.size(); ++i)
        {
            if (op->op == '+')
            {
                result += evaluate(op->children[i].get());
            }
            else if (op->op == '-')
            {
                result -= evaluate(op->children[i].get());
            }
            else if (op->op == '*')
            {
                result *= evaluate(op->children[i].get());
            }
            else if (op->op == '/')
            {
                double divisor = evaluate(op->children[i].get());
                if (divisor == 0.0)
                    throw runtime_error("division by zero");
                result /= divisor;
            }
        }
        return result;
    }
    else if (NumNode *num = dynamic_cast<NumNode *>(node))
    {
        return num->value;
    }

    throw runtime_error("Unknown node");
}

// Main Function
int main(int argc, const char **argv)
{
    string input;
    string text;
    vector<Token> tokens; // Assume this is filled with tokens after calling the lexer
    // ... Lexer call to fill the tokens ...
    while (getline(cin, input))
    {
        text += input;
        if (!cin.eof())
        {
            text += '\n';
        }
    }

    tokens = readTokens(text);

    Parser parser;
    try
    {
        auto ast = parser.parse(tokens);
        printInfix(ast.get());
        cout << endl;
        cout << evaluate(ast.get()) << endl;
    }
    catch (const runtime_error &e)
    {
        cerr << e.what() << endl;
        return 3; // Runtime error
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl;
        return 2; // Parse error
    }

    return 0;
}

int main(int argc, const char **argv)
{
    return 0;
}