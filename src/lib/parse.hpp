#include "lex.hpp"
#include <unordered_map>

struct Node
{
    Token token;
    vector<Node> children;
};

class AST
{
public:
    AST(const vector<Token> &tokens, int &index);
    ~AST();

    double evaluateAST(std::unordered_map<std::string, double>& variables);
    Node getRoot() const;
    void printInfix() const;
    void printAST(Node node, int depth);

private:
    Node root;

    double evaluate(Node root, std::unordered_map<std::string, double>& variables) const;
    Node makeTree(const vector<Token> &tokens, int &index);
    void checkTree(Node node, int childNum, int totalChildren, TokenType OPERATOR) const;
    void printInfix(const Node node) const;
};

void printErrorTwo(const Token &token);