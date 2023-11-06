#include "lex.hpp"
#include <unordered_map>

struct Node
{
    Token token;
    vector<Node *> children;    
};

// 4 node types that inherit form struct Node
//

struct IfElseNode : public Node
{
    Token token;
    Node* condition;
    bool hasElse;
    vector<Node *> statementsTrue;
    vector<Node *> statementsFalse;
};

struct WhileNode : public Node
{
    Token token;
    Node* condition;
    bool hasElse;
    vector<Node *> statements;
};

struct PrintNode : public Node
{
    Token token;
    Node* expression;
};

class AST
{
public:
    AST(const vector<Token> &tokens, int &index);
    ~AST();

    double evaluateAST(unordered_map<string, double> &variables);
    Node *getRoot() const;
    Node *makeNode(const Token &token);
    void printInfix() const;
    bool error;
    bool checkIden(Node *root, unordered_map<string, double> &variables);
    bool checkVar(Node *root);

private:
    Node *root;
    double evaluate(Node *root, unordered_map<string, double> &variables);
    
    Node *parseAll(const vector<Token> &tokens, int &index);

    Node *parseIf(const vector<Token> &tokens, int &index);
    Node *parseWhile(const vector<Token> &tokens, int &index);
    Node *parsePrint(const vector<Token> &tokens, int &index);

    Node *parseExpression(const vector<Token> &tokens, int &index);

    Node *parseAssignment(const std::vector<Token> &tokens, int &index);
    Node *parseComparison(const std::vector<Token> &tokens, int &index);
    Node *parseLogical(const std::vector<Token> &tokens, int &index);
    Node *parseEquality(const std::vector<Token> &tokens, int &index);
    Node *parseAddition(const std::vector<Token> &tokens, int &index);
    Node *parseMultiplication(const std::vector<Token> &tokens, int &index);
    Node *parsePrimary(const std::vector<Token> &tokens, int &index);
    bool match(const std::vector<Token> &tokens, int index, string expectedType);
    Node *makeTree(const vector<Token> &tokens, int &index);
    void deleteNode(Node *node);
    void printInfix(const Node *node) const;
};

void printError(const Token &token, bool &error);