#include "statements.hpp"
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>


void printAll(Node *node, int &depth) ;
void printInfix(const Node *node) ;
void printIfElse(const Node *node, int &depth) ;
void printWhile(const Node *node, int &depth) ;
void printPrint(const Node *node, int &depth) ;
