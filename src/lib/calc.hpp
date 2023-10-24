#include "lex.hpp"

struct Node
{
    Token token;
    vector<Node *> children;
};

class AST
{
public:
    AST(const vector<Token> &tokens);
    ~AST();

    double evaluateAST();
    Node *getRoot() const;
    int getPrecedence(string op) const;
    void printInfix() const;

private:
    Node *root;
    double evaluate(Node *root) const;
    Node *makeTree(const vector<Token> &tokens, int &index, int eindex);
    Node *makeNode(const Token &token);
    void checkTree(Node *node, int childNum, int totalChildren, TokenType OPERATOR) const;
    void deleteNode(Node *node);
    void printInfix(const Node *node) const;
};

void printErrorTwo(const Token &token);