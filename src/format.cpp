#include "lib/format.hpp"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

using namespace std;


void printAll(Node *node, int &depth)
{
    //printInfix(node);
    //cout << "node token text: " << node->token.text << endl;
    for(int j = 0; j < depth; j++)
    {
        cout << "   ";
    }
    if (IfElseNode *iENode = dynamic_cast<IfElseNode*>(node)) {
        //cout << "printing a if expression:" << endl;
        // Node is a WhileNode
        printIfElse(iENode, depth);
    } else if (WhileNode *wNode = dynamic_cast<WhileNode*>(node)) {
        // Node is an IfElseNode
        //cout << "printing a while expression:" << endl;
        printWhile(wNode, depth);
    } else if (PrintNode *pNode = dynamic_cast<PrintNode*>(node)) {
        //cout << "printing a print expression:" << endl;
        // Node is a PrintNode
        printPrint(pNode, depth);
    } else {
        //cout << "Printing a infix expression:" << endl;
        // Node is a normal Node
        printInfix(node, depth);
        cout << endl;
    }
}

// void printAll(const Node *node)
// {
//     if (typeid(*node) == typeid(IfElseNode)) {
//         // Node is a WhileNode
//         printWhile(node);
//     } else if (typeid(*node) == typeid(WhileNode)) {
//         // Node is an IfElseNode
//         printIfElse(node);
//     } else if (typeid(*node) == typeid(PrintNode)) {
//         // Node is a PrintNode
//         printPrint(node);
//     } else {
//         // Node is a normal Node
//         printInfix(node);
//     }
// }

void printIfElse(const Node *node, int &depth) 
{
    if (node)
        cout << "if ";
    const IfElseNode* ifElseNode = dynamic_cast<const IfElseNode*>(node);
    if (ifElseNode) {
        printInfix(ifElseNode->condition, depth);
        cout << " {" << endl;
        depth ++;
        for(size_t i = 0; i < ifElseNode->statementsTrue.size(); i++) {
            printAll(ifElseNode->statementsTrue[i], depth);
        }
        for(int j = 0; j < depth - 1; j++)
        {
            cout << "   ";
        }
        cout << "}" << endl;
        if(ifElseNode->hasElse) {
            for(int j = 0; j < depth - 1; j++)
            {
                cout << "   ";
            }
            cout << "else {" << endl;
            for(size_t i = 0; i < ifElseNode->statementsFalse.size(); i++) {
                printAll(ifElseNode->statementsFalse[i], depth);
            }
            for(int j = 0; j < depth - 1; j++)
            {
                cout << "   ";
            }
            cout << "}" << endl;
        }
    }
}

// void printIfElseHelper(const Node *node) const
// {

// }

void printWhile(const Node *node, int &depth) 
{
    if (node)
        cout << "while ";
    const WhileNode* whileNode = dynamic_cast<const WhileNode*>(node);
    if (whileNode) {
        printInfix(whileNode->condition, depth);
        cout << " {" << endl;
        depth ++;
        for(size_t i = 0; i < whileNode->statements.size(); i++) {
            printAll(whileNode->statements[i], depth);
        }
        cout << "}" << endl;
    }
}

// void printWhileHelper(const Node *node) const
// {

// }

void printPrint(const Node *node, int &depth) 
{
    if (node)
        cout << "print ";
    const PrintNode* printNode = dynamic_cast<const PrintNode*>(node);
    if (printNode) {
        printInfix(printNode->expression, depth);
        cout << endl;
    }
}

// void printPrintHelper(const Node *node) const
// {

// }

void printInfix(const Node *node, int &depth) 
{
    if (node && (node->token.type != FLOAT && node->token.type != IDENTIFIER && node->token.type != BOOLEAN))
        cout << "(";
    printInfixHelper(node);
    if (node && (node->token.type != FLOAT && node->token.type != IDENTIFIER && node->token.type != BOOLEAN))
        cout << ")";

}

// Prints the infix notation of a given AST.
void printInfixHelper(const Node *node)
{
    //cout << "in infix notation:" << endl;
    if (!node)
    {
        return;
    }
    else if (node->token.type == FLOAT)
    {
        //cout << "in infix notation float:" << endl;
        double val = stod(node->token.text);
        if (val == static_cast<int>(val))
            cout << static_cast<int>(val);
        else
            cout << node->token.text;
    }
    else if (node->token.type == IDENTIFIER || node->token.type == BOOLEAN)
    {

        //cout << "in infix notation iden:" << endl;
        cout << node->token.text;
    }
    else
    {
        //cout << "in infix notation else:" << endl;
        bool isFirst = true;
        // for (size_t i = 0; i < node->children.size(); i++)
        // {
        //     cout << "children text:" << node->children[i]->token.text << endl;
        // }
        //dafssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
        for (const auto &child : node->children)
        {
            //cout << "in infix notation before helper before ifs:" << endl;
            if (!child)
                continue;
            //cout << "in infix notation before past if 1:" << endl;
            if (!isFirst)
            {
                cout << " " << node->token.text << " ";
            }
            else
            {
                isFirst = false;
            }
            //cout << "in infix notation before past if 2:" << endl;
            if (child->token.type != FLOAT && child->token.type != IDENTIFIER && child->token.type != BOOLEAN)
            {
                cout << "(";
            }
            //cout << "in infix notation before helper:" << endl;
            printInfixHelper(child);
            if (child->token.type != FLOAT && child->token.type != IDENTIFIER && child->token.type != BOOLEAN)
            {
                cout << ")";
            }
        }
    }
    //cout << "in infix notation end of it:" << endl;
}


int main(int argc, const char **argv)
{
    string input;
    string text;
    vector<Token> tokens;

    while (getline(cin, input))
    {
        text += input;
        if (!cin.eof())
        {
            text += '\n';
        }
    }

    //test cases:
    //text = "x \n 42";
    //text = "x = 42";
    //text = "x = 42 \n 5 + 7";
    //text = "x = 42 \n steps = 0 \n while x > 1 { \n steps = steps + 1 \n if x % 2 == 0 { \n x = x / 2 \n } \n else { \n x = 3 * x + 1 \n } \n } \n print steps \n";
    //text = "x = 42 \n steps = 0 \n while x > 1 { \n steps = steps + 1 \n if x % 2 == 0 { \n x = x / 2 \n } \n else { \n x = 3 * x + 1 \n } \n } \n ";
    //text = "steps = 0 \n while steps < 3 { \n steps = steps + 1 \n 5 / 9 \n } \n 4 - 7 ";
    //text = "x = 42 \n steps = 0 \n if steps < 3 { \n steps = steps + 1 \n } \n else { \n x = 3 * x + 1 \n }";
    //text = "6 \n print 6";
    // lex
    tokens = readTokens(text);

    // set up variables for muti expression parsing
    
    int index = 0;
    vector<Node*> trees;
    std::unordered_map<std::string, double> variables;
    if (tokens.back().text == "error") //
    {
        exit(1);
    }
    //cout << "HERE" << endl;
    //parse the tokens and put into trees
    while(tokens[index].type != END)
    {
        Node *root;
        root = makeTree(tokens, index); 
        trees.push_back(root);
        index ++;
    } 
    //cout << "Trees length " << trees.size() << "" << endl;
    // cout << "Size: " << trees[0].getRoot()->children.size() << endl;
    // cout << "nullptr?: " << (trees[0].getRoot()->children[0] == nullptr) << endl;
    // cout << "Children text: " << trees[0].getRoot()->children[0]->token.text << endl; //line that causes error


    // for (size_t i = 0; i < trees.size(); i++)
    // {
    //     cout << "Root type: " << typeid(trees[i].getRoot()).name() << endl;
    // }
    //print and evaluate trees
    for (size_t i = 0; i < trees.size(); i++)
    {
        //cout << "right before segfault3" << endl;
        int depth = 0;
        printAll(trees[i], depth);
    }

    return 0;
}