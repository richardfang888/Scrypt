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
    AST(const vector<Token> &tokens);
    ~AST();

    double evaluateAST(unordered_map<string, double> &variables);
    Node *getRoot() const;
    Node *makeNode(const Token &token);
    void printInfix() const;
    bool error;
    bool checkTree(Node* root, unordered_map<string, double> &variables);

private:
    Node *root;
    double evaluate(Node *root, unordered_map<string, double> &variables);
    Node* parseInfix(const std::vector<Token>& tokens, int& index);
    Node* parseAssignment(const std::vector<Token>& tokens, int& index);
    Node* parseAddition(const std::vector<Token>& tokens, int& index);
    Node* parseMultiplication(const std::vector<Token>& tokens, int& index);
    Node* parsePrimary(const std::vector<Token>& tokens, int& index);
    bool match(const std::vector<Token>& tokens, int index, TokenType expectedType);
    Node *makeTree(const vector<Token> &tokens, int &index);
    void deleteNode(Node *node);
    void printInfix(const Node *node) const;
};

void printErrorTwo(const Token &token);