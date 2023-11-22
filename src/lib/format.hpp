#include "statements.hpp"
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>


void printAll(Node *node, int &depth);
void printInfix(Node *node, bool semi);
void printInfixHelper(Node *node);
void printIfElse(const Node *node, int &depth);
void printWhile(const Node *node, int &depth);
void printPrint(const Node *node);
void printFunctDef(const Node *node, int &depth);
void printReturn(const Node *node);
void printFunctCall(const Node *node);
