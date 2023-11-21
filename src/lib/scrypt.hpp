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
> {
    using variant<double, bool, nullptr_t, shared_ptr<vector<Value>>, shared_ptr<Function>>::variant;
};

class Function {
    public:
        FunctDefNode *function;
        unordered_map<string, Value> functVariables;
};

Value evaluateAll(Node *node, unordered_map<string, Value> &variables, bool &error);
Value evaluateExpression(Node* root, unordered_map<string, Value> &variables, bool &error);
Value evaluateIfElse(IfElseNode* root, unordered_map<string, Value> &variables, bool &error);
Value evaluateWhile(WhileNode* root, unordered_map<string, Value> &variables, bool &error);
Value evaluatePrint(PrintNode* root, unordered_map<string, Value> &variables, bool &error);
Value evaluateFunctDef(FunctDefNode* root, unordered_map<string, Value> &variables, bool &error);
Value evaluateFunctCall(FunctCallNode* root, unordered_map<string, Value> &variables, bool &error);
Value evaluateReturn(ReturnNode* root, unordered_map<string, Value> &variables, bool &error);