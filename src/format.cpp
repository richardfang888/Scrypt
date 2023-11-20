#include "lib/format.hpp"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

using namespace std;

//Prints a AST given its root node keeps track of depth for indentation
void printAll(Node *node, int &depth)
{
    for(int j = 0; j < depth; j++)
    {
        cout << "    ";
    }
    if (IfElseNode *iENode = dynamic_cast<IfElseNode*>(node)) {
        // Node is a WhileNode
        printIfElse(iENode, depth);
    } else if (WhileNode *wNode = dynamic_cast<WhileNode*>(node)) {
        // Node is an IfElseNode
        printWhile(wNode, depth);
    } else if (PrintNode *pNode = dynamic_cast<PrintNode*>(node)) {
        // Node is a PrintNode
        printPrint(pNode, depth);
    } else {
        // Node is a normal Node
        printInfix(node, true);
        cout << endl;
    }
}
// prints the if/else node
void printIfElse(const Node *node, int &depth) 
{
    if (node)
        cout << "if ";
    const IfElseNode* ifElseNode = dynamic_cast<const IfElseNode*>(node);
    if (ifElseNode) {
        printInfix(ifElseNode->condition, false);
        cout << " {" << endl;
        depth ++;
        for(size_t i = 0; i < ifElseNode->statementsTrue.size(); i++) {
            printAll(ifElseNode->statementsTrue[i], depth);
        }
        depth --;
        for(int j = 0; j < depth; j++)
        {
            cout << "    ";
        }
        cout << "}" << endl;
        if(ifElseNode->hasElse) {
            for(int j = 0; j < depth; j++)
            {
                cout << "    ";
            }
            cout << "else {" << endl;
            depth ++;
            for(size_t i = 0; i < ifElseNode->statementsFalse.size(); i++) {
                printAll(ifElseNode->statementsFalse[i], depth);
            }
            depth --;
            for(int j = 0; j < depth; j++)
            {
                cout << "    ";
            }
            cout << "}" << endl;
        }
    }
}
// prints the while node
void printWhile(const Node *node, int &depth) 
{
    if (node)
        cout << "while ";
    const WhileNode* whileNode = dynamic_cast<const WhileNode*>(node);
    if (whileNode) {
        printInfix(whileNode->condition, false);
        cout << " {" << endl;
        depth ++;
        for(size_t i = 0; i < whileNode->statements.size(); i++) {
            printAll(whileNode->statements[i], depth);
        }
        depth --;
        for(int j = 0; j < depth; j++)
        {
            cout << "    ";
        }
        cout << "}" << endl;
    }
}

// prints the print node
void printPrint(const Node *node, int &depth) 
{
    if (node)
        cout << "print ";
    const PrintNode* printNode = dynamic_cast<const PrintNode*>(node);
    if (printNode) {
        printInfix(printNode->expression, true);
        cout << endl;
    }
}
// Prints the infix notation of a given AST.
void printInfix(Node *node, bool semi) 
{
    bool isArrayAssignOrArrayLiteral = false;
    if(ArrayLiteralNode *aLNode = dynamic_cast<ArrayLiteralNode*>(node))
    {
        isArrayAssignOrArrayLiteral = true;
    }
    else if(ArrayAssignNode *aANode = dynamic_cast<ArrayAssignNode*>(node))
    {
        isArrayAssignOrArrayLiteral = true;
    }
    if (!isArrayAssignOrArrayLiteral && node && (node->token.type != FLOAT && node->token.type != IDENTIFIER && node->token.type != BOOLEAN))
        cout << "(";
    printInfixHelper(node);
    if (!isArrayAssignOrArrayLiteral && node && (node->token.type != FLOAT && node->token.type != IDENTIFIER && node->token.type != BOOLEAN))
        cout << ")";
    if (semi) {
        cout << ";";
    }
}

// Prints the infix notation of a given AST.
void printInfixHelper(Node *node)
{
    //cout << " |" << node->token.text << "| ";

    if (!node)
    {
        return;
    }
    else if (ArrayLiteralNode *aLNode = dynamic_cast<ArrayLiteralNode*>(node))
    {
        //cout << "SADFSADFlength: " << aLNode->array.size() << endl;
        //cout << " HERE|" << node->token.text << "|HERE ";
        cout << "[";
        bool isArrayAssignOrArrayLiteral = false;
        for (size_t i = 0; i < aLNode->array.size(); i++)
        {
            Node *currNode = aLNode->array[i];
            if(ArrayLiteralNode *aLNode = dynamic_cast<ArrayLiteralNode*>(currNode))
            {
                isArrayAssignOrArrayLiteral = true;
            }
            else if(ArrayAssignNode *aANode = dynamic_cast<ArrayAssignNode*>(node))
            {
                isArrayAssignOrArrayLiteral = true;
            }
            if (!isArrayAssignOrArrayLiteral && currNode && (currNode->token.type != FLOAT && currNode->token.type != IDENTIFIER && currNode->token.type != BOOLEAN))
                cout << "(";
            printInfixHelper(currNode);
            if (!isArrayAssignOrArrayLiteral && currNode && (currNode->token.type != FLOAT && currNode->token.type != IDENTIFIER && currNode->token.type != BOOLEAN))
                cout << ")";
            if (i != aLNode->array.size() - 1)
            {
                cout << ", ";
            }
        }
        cout << "]";
    }
    else if (ArrayAssignNode *aANode = dynamic_cast<ArrayAssignNode*>(node))
    {
        cout << aANode->token.text << "[";
        printInfixHelper(aANode->arrayIndex);
        cout << "]";
    }
    else if (node->token.type == FLOAT)
    {
        double val = stod(node->token.text);
        if (val == static_cast<int>(val))
            cout << static_cast<int>(val);
        else
            cout << node->token.text;
    }
    else if (node->token.type == IDENTIFIER || node->token.type == BOOLEAN)
    {

        cout << node->token.text;
    }
    else
    {
        bool isFirst = true;
        for (const auto &child : node->children)
        {
            if (!child)
                continue;
            if (!isFirst)
            {
                cout << " " << node->token.text << " ";
            }
            else
            {
                isFirst = false;
            }
            bool isArrayLit = false;
            if(ArrayLiteralNode *aLNode = dynamic_cast<ArrayLiteralNode*>(child))
            {
                isArrayLit = true;
            }
            if (!isArrayLit && child->token.type != FLOAT && child->token.type != IDENTIFIER && child->token.type != BOOLEAN)
            {
                cout << "(";
            }
            printInfixHelper(child);
            if (!isArrayLit && child->token.type != FLOAT && child->token.type != IDENTIFIER && child->token.type != BOOLEAN)
            {
                cout << ")";
            }
        }
    }
}

int main(int argc, const char **argv)
{
    string input;
    string text;
    vector<Token> tokens;

    // while (getline(cin, input))
    // {
    //     text += input;
    //     if (!cin.eof())
    //     {
    //         text += '\n';
    //     }
    // }

    //text = "x = 42; \n steps = 0; \n while x > 1 { \n steps = steps + 1; \n if x % 2 == 0 { \n x = x / 2; \n } \n else { \n x = 3 * x + 1; \n } \n } \n ";
    //text = "x=2;\n if x==1 \n{print 1;\n} else \n{print 0;\n}\n";
    //text = "print a = 49; \n print b = 21; \n while a != b {\n if a > b {\n a = a - b; \n } \n else if b > a {\n b = b - a; \n } \n } \n print a; \n";
    text = "array = [true, 2, 1+1+1, 4, [5]]; \n print array[2]; \n print array;";
    //text = "array = [true, 2, 1+1+1, 4];";
    //text = "print array[2];";
    //text = "array = [true, 2, 1+1+1, 4]; \n print array[2]; \n print array;";
    //text = "array = [true, 2 + 1, [5]];";

    tokens = readTokens(text);

    // set up variables for muti expression parsing
    
    int index = 0;
    vector<Node*> trees;
    if (tokens.empty() || tokens.back().text == "error") //
    {
        exit(1);
    }

    //parse the tokens and put into trees
    while(tokens[index].type != END)
    {
        Node *root;
        root = makeTree(tokens, index); 
        trees.push_back(root);
        index ++;
    } 
    cout << "MADE IT PAST PARSING" << endl;
    //print and evaluate trees
    for (size_t i = 0; i < trees.size(); i++)
    {
        int depth = 0;
        printAll(trees[i], depth);
    }
    //delete trees
    for (size_t i = 0; i < trees.size(); i++)
    {  
        deleteNodeAll(trees[i]);
    }
    return 0;
}