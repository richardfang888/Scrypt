#include "statements.hpp"
#include <iostream>

variant<double, bool> evaluateAll(Node *node, unordered_map<string, variant<double, bool>> &variables, vector<FunctDefNode *> &functions, bool &error);
variant<double, bool> evaluateExpression(Node* root, unordered_map<string, variant<double, bool>> &variables, vector<FunctDefNode *> &functions, bool &error);
variant<double, bool> evaluateIfElse(IfElseNode* root, unordered_map<string, variant<double, bool>> &variables, vector<FunctDefNode *> &functions, bool &error);
variant<double, bool> evaluateWhile(WhileNode* root, unordered_map<string, variant<double, bool>> &variables, vector<FunctDefNode *> &functions, bool &error);
variant<double, bool> evaluatePrint(PrintNode* root, unordered_map<string, variant<double, bool>> &variables, vector<FunctDefNode *> &functions, bool &error);
variant<double, bool> evaluateFunctDef(FunctDefNode* root, unordered_map<string, variant<double, bool>> &variables, vector<FunctDefNode *> &functions, bool &error);
variant<double, bool> evaluateFunctCall(FunctCallNode* root, unordered_map<string, variant<double, bool>> &variables, vector<FunctDefNode *> &functions, bool &error);
variant<double, bool> evaluateReturn(ReturnNode* root, unordered_map<string, variant<double, bool>> &variables, bool &error);
variant<double, bool> evaluateStatements(vector<Node*> &statements, unordered_map<string, variant<double, bool>> &variables, vector<FunctDefNode *> &functions, bool &error);
FunctCallNode * getFunction(vector<FunctDefNode *> &functions, FunctCallNode *node, unordered_map<string, variant<double, bool>> &variables);