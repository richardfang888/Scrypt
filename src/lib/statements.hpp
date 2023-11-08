#include "lex.hpp"
#include <unordered_map>
#include <variant>
struct Node
{
    Token token;
    vector<Node *> children;
    virtual ~Node() = default;
};

struct IfElseNode : public Node
{
    Token token;
    Node* condition;
    bool hasElse;
    vector<Node *> statementsTrue;
    vector<Node *> statementsFalse;
    virtual ~IfElseNode() = default;
};

struct WhileNode : public Node
{
    Token token;
    Node* condition;
    vector<Node *> statements;
    virtual ~WhileNode() = default;
};

struct PrintNode : public Node
{
    Token token;
    Node* expression;
    virtual ~PrintNode() = default;
};

class AST
{
public:
    AST(const vector<Token> &tokens, int &index);
    ~AST();

    variant<double, bool> evaluateAST(unordered_map<string, variant<double, bool>> &variables);
    Node *getRoot() const;
    Node *makeNode(const Token &token);
    WhileNode *makeWhileNode(const Token &token);

    void printAll() ;
    void printInfix(const Node *node) ;
    void printIfElse(const Node *node) ;
    void printWhile(const Node *node) ;
    void printPrint(const Node *node) ;
    bool error;
    bool checkIden(Node *root, unordered_map<string, variant<double, bool>> &variables);
    bool checkVar(Node *root);
    bool checkParen(vector<Token> &tokens);

private:
    Node *root;
    variant<double, bool> evaluate(Node *root, unordered_map<string, variant<double, bool>> &variables);

    Node *parseAll(const vector<Token> &tokens, int &index);

    IfElseNode *parseIf(const vector<Token> &tokens, int &index);
    WhileNode *parseWhile(const vector<Token> &tokens, int &index);
    PrintNode *parsePrint(const vector<Token> &tokens, int &index);

    Node *parseExpression(const vector<Token> &tokens, int &index);

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
    void printAll(const Node *node) ;
    void printInfixHelper(const Node *node) const;
    // void printIfElseHelper(const Node *node) const;
    // void printWhileHelper(const Node *node) const;
    // void printPrintHelper(const Node *node) const;
};

void printError(const Token &token, bool &error);