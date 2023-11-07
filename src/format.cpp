#include "lib/format.hpp"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

using namespace std;


void AST::printAll() const
{
    if (typeid(root) == typeid(IfElseNode)) {
        // Node is a WhileNode
        printWhile(root);
    } else if (typeid(root) == typeid(WhileNode)) {
        // Node is an IfElseNode
        printIfElse(root);
    } else if (typeid(root) == typeid(PrintNode)) {
        // Node is a PrintNode
        printPrint(root);
    } else {
        // Node is a normal Node
        printInfix(root);
    }
}

void AST::printAll(const Node node) const
{
    if (typeid(node) == typeid(IfElseNode)) {
        // Node is a WhileNode
        printWhile(node);
    } else if (typeid(node) == typeid(WhileNode)) {
        // Node is an IfElseNode
        printIfElse(node);
    } else if (typeid(node) == typeid(PrintNode)) {
        // Node is a PrintNode
        printPrint(node);
    } else {
        // Node is a normal Node
        printInfix(node);
    }
}

void AST::printIfElse(const Node node) const
{
    cout << "if ";
    const IfElseNode ifElseNode = static_cast<const IfElseNode&>(node);

    printInfix(ifElseNode.condition);
    cout << " {" << endl;
    for(int i = 0; i < ifElseNode.statementsTrue.size() - 1; i++) {
        printAll(ifElseNode.statementsTrue[i]);
    }
    cout << "}" << endl;
    if(ifElseNode.hasElse) {
        cout << "else {" << endl;
        for(int i = 0; i < ifElseNode.statementsFalse.size() - 1; i++) {
            printAll(ifElseNode.statementsFalse[i]);
        }
        cout << "}" << endl;
    }
}

void AST::printIfElseHelper(const Node node) const
{
    
}

void AST::printWhile(const Node node) const
{
    cout << "while ";
    const WhileNode whileNode = static_cast<const WhileNode&>(node);

    printInfix(whileNode.condition);
    cout << " {" << endl;
    for(int i = 0; i < whileNode.statements.size() - 1; i++) {
        printAll(whileNode.statements[i]);
    }
    cout << "}" << endl;

}

void AST::printWhileHelper(const Node node) const
{
    
}

void AST::printPrint(const Node node) const
{
    cout << "print ";
    const PrintNode printNode = static_cast<const PrintNode&>(node);
    
    printInfix(printNode.expression);
    cout << endl;

}

void AST::printPrintHelper(const Node node) const
{

}

void AST::printInfix(const Node node) const
{
    if ((node.token.type != FLOAT && node.token.type != IDENTIFIER && node.token.type != BOOLEAN))
        cout << "(";

    printInfixHelper(node);
    if ((node.token.type != FLOAT && node.token.type != IDENTIFIER && node.token.type != BOOLEAN))
        cout << ")";

    cout << endl;
}

// Prints the infix notation of a given AST.
void AST::printInfixHelper(const Node node) const
{

    if (node.token.type == FLOAT)
    {
        double val = stod(node.token.text);
        if (val == static_cast<int>(val))
            cout << static_cast<int>(val);
        else
            cout << node.token.text;
    }
    else if (node.token.type == IDENTIFIER || node.token.type == BOOLEAN)
    {
        cout << node.token.text;
    }
    else
    {
        bool isFirst = true;
        for (const auto &child : node.children)
        {
            if (!isFirst)
            {
                cout << " " << node.token.text << " ";
            }
            else
            {
                isFirst = false;
            }
            if (child.token.type != FLOAT && child.token.type != IDENTIFIER && child.token.type != BOOLEAN)
            {
                cout << "(";
            }
            printInfixHelper(child);
            if (child.token.type != FLOAT && child.token.type != IDENTIFIER && child.token.type != BOOLEAN)
            {
                cout << ")";
            }
        }
    }
}


int main(int argc, const char argv)
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

    //test cases:
    text = "x = 42";

    // lex
    tokens = readTokens(text);

    // set up variables for muti expression parsing
    
    int index = 0;
    vector<AST> trees;
    std::unordered_map<std::string, double> variables;
    if (tokens.back().text == "error")
    {
        exit(1);
    }
    
    //parse the tokens and put into trees
    while(tokens[index].type != END)
    {
        cout << index << endl;
        AST ast(tokens, index);
        trees.push_back(ast);
    }
    //print and evaluate trees
    for(auto tree : trees)
    {
        //tree.printAll();
    }

    return 0;
}