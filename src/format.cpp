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
    }
    else if (WhileNode *wNode = dynamic_cast<WhileNode*>(node)) {
        // Node is an IfElseNode
        printWhile(wNode, depth);
    } 
    else if (PrintNode *pNode = dynamic_cast<PrintNode*>(node)) {
        // Node is a PrintNode
        printPrint(pNode);
    } 
    else if (FunctDefNode *fdNode = dynamic_cast<FunctDefNode*>(node)) {
        // Node is a FunctDefNode
        printFunctDef(fdNode, depth);
    } 
    else if (ReturnNode *rNode = dynamic_cast<ReturnNode*>(node)) {
        // Node is a ReturnNode
        printReturn(rNode);
    } 
    else {
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
        for (size_t i = 0; i < ifElseNode->statementsTrue.size(); i++) {
            printAll(ifElseNode->statementsTrue[i], depth);
        }
        depth --;
        for (int j = 0; j < depth; j++) {
            cout << "    ";
        }
        cout << "}" << endl;
        if (ifElseNode->hasElse) {
            for(int j = 0; j < depth; j++) {
                cout << "    ";
            }
            cout << "else {" << endl;
            depth ++;
            for (size_t i = 0; i < ifElseNode->statementsFalse.size(); i++) {
                printAll(ifElseNode->statementsFalse[i], depth);
            }
            depth --;
            for (int j = 0; j < depth; j++) {
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
        for (size_t i = 0; i < whileNode->statements.size(); i++) {
            printAll(whileNode->statements[i], depth);
        }
        depth --;
        for (int j = 0; j < depth; j++)
        {
            cout << "    ";
        }
        cout << "}" << endl;
    }
}

// prints the print node
void printPrint(const Node *node) 
{
    if (node)
        cout << "print ";
    const PrintNode* printNode = dynamic_cast<const PrintNode*>(node);
    if (printNode) {
        printInfix(printNode->expression, true);
        cout << endl;
    }
}

void printFunctDef(const Node *node, int &depth)
{
    if (node)
        cout << "def ";
    const FunctDefNode* functDefNode = dynamic_cast<const FunctDefNode*>(node);
    if (functDefNode) {
        cout << functDefNode->functname.text << "(";
        for (size_t i = 0; i < functDefNode->params.size(); i++) {
            cout << functDefNode->params[i];
            if (i != functDefNode->params.size() - 1) {
                cout << ", ";
            }
        }
        cout << ") ";
        cout << "{" << endl;
        depth ++;
        for (size_t i = 0; i < functDefNode->statements.size(); i++) {
            printAll(functDefNode->statements[i], depth);
        }
        depth --;
        for (int j = 0; j < depth; j++)
        {
            cout << "    ";
        }
        cout << "}" << endl;
    }
}

void printReturn(const Node *node)
{
    if (node)
        cout << "return";
    const ReturnNode* returnNode = dynamic_cast<const ReturnNode*>(node);
    if (returnNode) {
        if (returnNode->expression) {
            cout << " ";
            printInfix(returnNode->expression, true);
        }
        else {
            cout << ";";
        }
        cout << endl;
    }
}

void printFunctCall(const Node *node)
{
    const FunctCallNode* functCallNode = dynamic_cast<const FunctCallNode*>(node);
    if (functCallNode) {
        cout << functCallNode->functname.text << "(";
        for (size_t i = 0; i < functCallNode->arguments.size(); i++) {
            printInfix(functCallNode->arguments[i], false);
            if (i != functCallNode->arguments.size() - 1) {
                cout << ", ";
            }
        }
    }
}


// Prints the infix notation of a given AST.
void printInfix(Node *node, bool semi) 
{
    bool isFunctCall = false;
    bool isArrayAssignOrArrayLiteral = false;

    if(dynamic_cast<ArrayLiteralNode*>(node))
    {
        isArrayAssignOrArrayLiteral = true;
    }
    else if(dynamic_cast<ArrayAssignNode*>(node))
    {
        isArrayAssignOrArrayLiteral = true;
    }
    if (dynamic_cast<const FunctCallNode*>(node)) {
        isFunctCall = true;
    }
    if (!isArrayAssignOrArrayLiteral && !isFunctCall && node && (node->token.type != FLOAT && node->token.type != IDENTIFIER && node->token.type != BOOLEAN && node->token.type != NULLVAL)) 
    {
        cout << "(";
    }
    printInfixHelper(node);
    if (!isArrayAssignOrArrayLiteral &&!isFunctCall && node && (node->token.type != FLOAT && node->token.type != IDENTIFIER && node->token.type != BOOLEAN && node->token.type != NULLVAL)) 
    {
        cout << ")";
    }
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
    else if (const FunctCallNode *fcNode = dynamic_cast<const FunctCallNode*>(node))
    {
        printFunctCall(fcNode);
    }
    else if (ArrayLiteralNode *aLNode = dynamic_cast<ArrayLiteralNode*>(node))
    {
        //cout << "SADFSADFlength: " << aLNode->array.size() << endl;
        //cout << " HERE|" << node->token.text << "|HERE ";
        cout << "[";
        for (size_t i = 0; i < aLNode->array.size(); i++)
        {
            Node *currNode = aLNode->array[i];
            printInfix(currNode, false);
            if (i != aLNode->array.size() - 1)
            {
                cout << ", ";
            }
        }
        cout << "]";
    }
    else if (ArrayAssignNode *aANode = dynamic_cast<ArrayAssignNode*>(node))
    {
        printInfixHelper(aANode->expression);
        cout << "[";
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
    else if (node->token.type == IDENTIFIER || node->token.type == BOOLEAN || node->token.type == NULLVAL)
    {
        cout << node->token.text;
    }
    else
    {
        bool isFirst = true;
        //cout << "node text: " << node->token.text << endl;
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
            printInfix(child, false);
        }
    }
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

    // text = "x=2;\n if x==1 \n{print 1;\n} else \n{print 0;\n}\n";
    // text = "print a = 49; \n print b = 21; \n while a != b {\n if a > b {\n a = a - b; \n } \n else if b > a {\n b = b - a; \n } \n } \n print a; \n";
    
    // function tests
    // text = "def noop1(){}";
    // text = "def noop2(){return;}";
    // text = "if x == 1 {print 2;}";
    // text = "def noop3(){return null;}";
    // text = "def print_if_bool(val) {\nif val == true | val == false {\nprint val;\n}\n}\n";
    // text = "def make_mac(factor, scalar){def multiply_and_accumulate(value){return factor * value + scalar;\n}\nreturn multiply_and_accumulate;\n}\n";
    // text = "doit();";
    // text = "twelve = 3 * four();";
    // text = "forty1 = forty(1);";
    // text = "print max(1, 2, 3, four());";
    // text = "z = 42;\n\n def foo(x, y) {\n def square(value) {\nreturn value * value;\n}\nprint square(x + y + z);\n\n}\nz = 108;\nf = foo;\nresult = f(1, 2);\nif result != null {\nprint result;\n}\n";

    // error tests
    // text = "x = foo(,)";

    //text = "array = [true, 2, 1+1+1, 4, [5]]; \n print array[2]; \n print array;";
    //text = "array = [true, 2, 1+1+1, 4];";
    //text = "print array[2];"lo;
    //text = "array = [true, 2, 1+1+1, 4]; \n print array[2]; \n print array;";
    //text = "array = [true, 2 + 1, [5]];";

    //text = "array = [true, 2, 1+1+1, 4, [5]]; \n print array[2]; \n print array; \n \n arref = array; \n temp  = arref[1]; \n arref[1] = 0 - arref[3]; \n arref[3] = 0 - temp; \n print array;";
    //text = "arref[1] = 0 - arref[3];";
    //text = "value = 6; \n arret[1] = 5;";

    //text = "[1, 2, 3][ 2 ]; \n sum = x[1] + y[2] + z[3]; \n print [ \n true, \n false \n ][fake_bool];";
    //text = "[1, 2, 3][ 2 ];";
    //text = "print [1, 2, 3][ 2 ];";
    //text = "print [ \n true, \n false \n ][fake_bool];";
    //text = "print [ true, false ][fake_bool];";

    //text = "[]; \n [1]; \n [true,false]; \n x = [1, [2], [[3]]]; \n print     [4, 5, 6];";
    //text = "x = [1, [2], [[3]]]; ";
    //text = "x = [1, [2], [3]]; "; 

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
    //out << "MADE IT PAST PARSING" << endl;
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