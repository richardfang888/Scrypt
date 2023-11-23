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

struct ArrayLiteralNode : public Node
{
    Token token;
    vector<Node *> array;
    virtual ~ArrayLiteralNode() = default;
};

struct ArrayAssignNode : public Node
{
    Token token;
    Node *expression;
    Node *arrayIndex;
    virtual ~ArrayAssignNode() = default;
};

struct FunctDefNode : public Node
{
    Token token;
    Token functname;
    unordered_map<string, variant<double, bool>> vars;
    vector<string> params;
    vector<Node *> statements;
    virtual ~FunctDefNode() = default;
};

struct FunctCallNode : public Node
{
    Token functname;
    vector<Node *> arguments;
    virtual ~FunctCallNode() = default;
};

struct ReturnNode : public Node
{
    Token token;
    Node* expression;
    virtual ~ReturnNode() = default;
};

class Function;

struct Value : public variant<
    double,
    bool,
    nullptr_t,
    shared_ptr<vector<Value>>,
    shared_ptr<Function>
>{
    // using variant<double, bool, nullptr_t, shared_ptr<vector<Value>>>::variant;
    // bool operator==(const Value &other) const;
    // bool operator!=(const Value &other) const;

    // bool operator<(const Value &other) const;
    // bool operator>(const Value &other) const;
    // bool operator<=(const Value &other) const;
    // bool operator>=(const Value &other) const;
};

// using Array = shared_ptr<vector<Value>>;

class Function {
    public:
        FunctDefNode *function;
        unordered_map<string, Value> functVariables;
};


Node *makeNodeCalc(const Token &token);

ArrayLiteralNode *makeArrayLiteralNodeCalc(const Token &token);
ArrayAssignNode *makeArrayAssignNodeCalc(const Token &token);

bool checkIdenCalc(Node *root, unordered_map<string, Value> &variables, bool &error);
bool checkVarCalc(Node *root, bool &error);
bool checkParenCalc(vector<Token> &tokens, bool &error);

Node *parseExpressionCalc(const vector<Token> &tokens, int &index, bool &error);

Node *parseAssignmentCalc(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseComparisonCalc(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseLogicAndCalc(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseLogicXorCalc(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseLogicOrCalc(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseEqualityCalc(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseAddSubCalc(const std::vector<Token> &tokens, int &index, bool &error);
Node *parseMultDivModCalc(const std::vector<Token> &tokens, int &index, bool &error);
Node *parsePrimaryCalc(const std::vector<Token> &tokens, int &index, bool &error);

ArrayLiteralNode *parseArrayLiteralCalc(const std::vector<Token> &tokens, int &index, bool &error);
ArrayAssignNode *parseArrayAssignCalc(const std::vector<Token> &tokens, int &index, bool &error);

bool matchCalc(const std::vector<Token> &tokens, int index, string expectedType);
Node *makeTreeCalc(const vector<Token> &tokens, int &index, bool &error);
void deleteNodeCalc(Node *node);
void deleteNodeAllCalc(Node *node);

void printInfixCalc(Node *node);
void printInfixHelperCalc(Node *node);

void printErrorCalc(const Token &token, bool &error);

Value evaluateAllCalc(Node *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateExpressionCalc(Node* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateFunctDefCalc(FunctDefNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateFunctCallCalc(FunctCallNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateReturnCalc(ReturnNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateArrayLiteralCalc(ArrayLiteralNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateArrayAssignCalc(ArrayAssignNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct, bool setValue);
Value evaluateUtilityFunctCalc(FunctCallNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);

void printValueCalc(Value value);
Value lenCalc(Value array);
Value pushCalc(Value array, Value value);
Value popCalc(Value array);