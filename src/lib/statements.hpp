#include "lex.hpp"
#include <unordered_map>
#include <variant>
#include <memory>
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

struct ArrayLiteralNode : public Node
{
    Token token;
    vector<Node*> array;
    virtual ~ArrayLiteralNode() = default;
};

struct ArrayLookupNode : public Node
{
    Token token;
    Node* index;
    ArrayLiteralNode* refArray;
    virtual ~ArrayLookupNode() = default;
};

struct ArrayAssignNode : public Node
{
    Token token;
    Node* arrayIndex;
    //ArrayLiteralNode* refArray; // needed idk
    virtual ~ArrayAssignNode() = default;
};

Node *makeNode(const Token &token);
WhileNode *makeWhileNode(const Token &token);
IfElseNode *makeIfElseNode(const Token &token);
PrintNode *makePrintNode(const Token &token);
ArrayLiteralNode *makeArrayLiteralNode(const Token &token);
//ArrayLookupNode *makeArrayLookupNode(const Token &token);
ArrayAssignNode *makeArrayAssignNode(const Token &token);
bool checkIden(Node *root, unordered_map<string, variant<double, bool>> &variables, bool &error);
bool checkVar(Node *root, bool &error);
bool checkParen(vector<Token> &tokens, bool &error);
Node *parseAll(const vector<Token> &tokens, int &index, bool &error);
IfElseNode *parseIf(const vector<Token> &tokens, int &index, bool &error);
WhileNode *parseWhile(const vector<Token> &tokens, int &index, bool &error);
PrintNode *parsePrint(const vector<Token> &tokens, int &index, bool &error);
Node *parseExpression(const vector<Token> &tokens, int &index, bool checkSemi, bool &error);
//Node *parseExpressionInArray(const vector<Token> &tokens, int &index, bool checkSemi, bool &error);
Node *parseAssignment(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseComparison(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseLogicAnd(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseLogicXor(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseLogicOr(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseEquality(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseAddSub(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseMultDivMod(const std::vector<Token> &tokens, int &index, bool &error);
Node *parsePrimary(const std::vector<Token> &tokens, int &index, bool &error);

ArrayLiteralNode *parseArrayLiteral(const std::vector<Token> &tokens, int &index, bool &error);
ArrayAssignNode *parseArrayAssign(const std::vector<Token> &tokens, int &index, bool &error);

bool match(const std::vector<Token> &tokens, int index, string expectedType);
Node *makeTree(const vector<Token> &tokens, int &index);
void deleteNode(Node *node);
void deleteNodeAll(Node *node);
void printErrorStatement(const Token &token, bool &error);