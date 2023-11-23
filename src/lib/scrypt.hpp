#include "statements.hpp"
#include <memory>
#include <iostream>

class Function;

struct Value: public variant <
    double,
    bool,
    nullptr_t,
    shared_ptr<vector<Value>>,
    shared_ptr<Function>
> {};

// using Value = variant<double, bool, nullptr_t, shared_ptr<vector<Value>>, shared_ptr<Function>>;

class Function {
    public:
        FunctDefNode *function;
        unordered_map<string, Value> functVariables;
};

Value evaluateAll(Node *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateExpression(Node* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateIfElse(IfElseNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateWhile(WhileNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluatePrint(PrintNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateFunctDef(FunctDefNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateFunctCall(FunctCallNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateReturn(ReturnNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateArrayLiteral(ArrayLiteralNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);
Value evaluateArrayAssign(ArrayAssignNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct, bool setValue);
Value evaluateUtilityFunct(FunctCallNode* root, unordered_map<string, Value> &variables, bool &error, bool &inFunct);

void printValue(Value value);
Value len(Value array);
Value push(Value array, Value value);
Value pop(Value array);

