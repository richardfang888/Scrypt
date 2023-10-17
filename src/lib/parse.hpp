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
    void printInfix() const;

private:
    Node *root;

    double evaluate(Node *root) const;
    Node *makeTree(const vector<Token> &tokens, int &index, int eindex);
    void deleteNode(Node *node);
    void printInfix(const Node *node) const;
};