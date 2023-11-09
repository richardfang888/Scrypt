#include "statements.hpp"
#include <iostream>

void evaluateAll(Node *node, unordered_map<string, variant<double, bool>> &variables);
variant<double, bool> evaluateExpression(Node* root, unordered_map<string, variant<double, bool>> &variables, bool &error);
variant<double, bool> evaluateIfElse(IfElseNode* root, unordered_map<string, variant<double, bool>> &variables);
variant<double, bool> evaluateWhile(WhileNode* root, unordered_map<string, variant<double, bool>> &variables);
variant<double, bool> evaluatePrint(PrintNode* root, unordered_map<string, variant<double, bool>> &variables);