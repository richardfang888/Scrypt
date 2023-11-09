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

 variant<double, bool> evaluateAST(unordered_map<string, variant<double, bool>> &variables);
    //Node *getRoot() const;

    Node *makeNode(const Token &token);
    WhileNode *makeWhileNode(const Token &token);
    IfElseNode *makeIfElseNode(const Token &token);
    PrintNode *makePrintNode(const Token &token);

    void printAll(Node *node, int &depth) ;
    void printInfix(const Node *node) ;
    void printIfElse(const Node *node, int &depth) ;
    void printWhile(const Node *node, int &depth) ;
    void printPrint(const Node *node, int &depth) ;
    //bool error;
    bool checkIden(Node *root, unordered_map<string, variant<double, bool>> &variables, bool &error);
    bool checkVar(Node *root, bool &error);
    bool checkParen(vector<Token> &tokens, bool &error);

    //Node *root;
    variant<double, bool> evaluate(Node *root, unordered_map<string, variant<double, bool>> &variables);

    Node *parseAll(const vector<Token> &tokens, int &index, bool &error);

    IfElseNode *parseIf(const vector<Token> &tokens, int &index, bool &error);
    WhileNode *parseWhile(const vector<Token> &tokens, int &index, bool &error);
    PrintNode *parsePrint(const vector<Token> &tokens, int &index, bool &error);

    Node *parseExpression(const vector<Token> &tokens, int &index, bool &error);

    Node *parseAssignment(const std::vector<Token> &tokens, int &index, bool &error);
    Node *parseComparison(const std::vector<Token> &tokens, int &index, bool &error);
    Node *parseLogicAnd(const std::vector<Token> &tokens, int &index, bool &error);
    Node *parseLogicXor(const std::vector<Token> &tokens, int &index, bool &error);
    Node *parseLogicOr(const std::vector<Token> &tokens, int &index, bool &error);
    Node *parseEquality(const std::vector<Token> &tokens, int &index, bool &error);
    Node *parseAddSub(const std::vector<Token> &tokens, int &index, bool &error);
    Node *parseMultDivMod(const std::vector<Token> &tokens, int &index, bool &error);
    Node *parsePrimary(const std::vector<Token> &tokens, int &index, bool &error);
    bool match(const std::vector<Token> &tokens, int index, string expectedType);
    Node *makeTree(const vector<Token> &tokens, int &index);
    void deleteNode(Node *node);
    void printInfixHelper(const Node *node);
    // void printIfElseHelper(const Node *node) ;
    // void printWhileHelper(const Node *node) ;
    // void printPrintHelper(const Node *node) ;


void printError(const Token &token, bool &error);