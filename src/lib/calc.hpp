#include "lex.hpp"
#include <unordered_map>
#include <variant>

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

    variant<double, bool> evaluateAST(unordered_map<string, variant<double, bool>> &variables);
    Node *getRoot() const;
    Node *makeNode(const Token &token);
    void printInfix(const Node *node) const;
    bool error;
    bool checkIden(Node *root, unordered_map<string, variant<double, bool>> &variables);
    bool checkVar(Node *root);
    bool checkParen(vector<Token> &tokens);

private:
    Node *root;
    variant<double, bool> evaluate(Node *root, unordered_map<string, variant<double, bool>> &variables);
    Node *parseAssignment(const std::vector<Token> &tokens, int &index);
    Node *parseComparison(const std::vector<Token> &tokens, int &index);
    Node *parseLogicAnd(const std::vector<Token> &tokens, int &index);
    Node *parseLogicXor(const std::vector<Token> &tokens, int &index);
    Node *parseLogicOr(const std::vector<Token> &tokens, int &index);
    Node *parseEquality(const std::vector<Token> &tokens, int &index);
    Node *parseAddSub(const std::vector<Token> &tokens, int &index);
    Node *parseMultDivMod(const std::vector<Token> &tokens, int &index);
    Node *parsePrimary(const std::vector<Token> &tokens, int &index);
    bool match(const std::vector<Token> &tokens, int index, string expectedType);
    Node *makeTree(const vector<Token> &tokens, int &index);
    void deleteNode(Node *node);
    void printInfix(const Node *node) const;
};

void printError(const Token &token, bool &error);