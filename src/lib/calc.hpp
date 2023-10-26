#include "lex.hpp"
#include <unordered_map>

struct Node
{
    Token token;
    vector<Node *> children;
};

class AST
{
public:
    AST(const vector<Token> &tokens, bool &error);
    ~AST();

    double evaluateAST(unordered_map<string, double> &variables);
    Node *getRoot() const;
    Node *makeNode(const Token &token);
    void printInfix() const;

private:
    Node *root;
    double evaluate(Node *root, unordered_map<string, double> &variables) const;
    Node* parseInfix(const std::vector<Token>& tokens, int& index, bool& error);
    Node* parseAssignment(const std::vector<Token>& tokens, int& index, bool& error);
    Node* parseAddition(const std::vector<Token>& tokens, int& index, bool& error);
    Node* parseMultiplication(const std::vector<Token>& tokens, int& index, bool& error);
    Node* parsePrimary(const std::vector<Token>& tokens, int& index, bool& error);
    bool match(const std::vector<Token>& tokens, int index, TokenType expectedType);
    Node *makeTree(const vector<Token> &tokens, int &index, bool &error);
    void checkTree(Node *node, int childNum, int totalChildren, TokenType OPERATOR) const;
    void deleteNode(Node *node);
    void printInfix(const Node *node) const;
};

void printErrorTwo(const Token &token);