#include "lib/parse.hpp"
#include <iostream>

using namespace std;

// Constructor that creates an AST from a list of tokens
AST::AST(const vector<Token> &tokens, int &index)
{
    if (tokens.empty())
    {
        printErrorTwo(Token{END, "", 0, 1, 1});
        return;
    }

    root = makeTree(tokens, index);
}

// for debugging purposes (to see the created tree):
// void AST::printAST(Node node, int depth) {
//     // Print the value of the current node
//     cout << node.token.text << "|" << depth << "|||";
//     // Recursively print the left and right subtrees
//     for (Node child : node.children)
//     {
//         printAST(child, depth + 1);
//     }
// }

AST::~AST(){ }

// Recursively creates an AST from a list of tokens,
// checks for if there are formatting errors on converted S expression
Node AST::makeTree(const vector<Token> &tokens, int &index)
{
    Token token = tokens[index];
    // Base case: if the token is a FLOAT, simply return a new node with that token.
    if (token.type == FLOAT)
    {
        Node node;
        node.token = token;
        index++;
        return node;
    }
    // Base case: if the token is a IDENTIFIER, simply return a new node with that token.
    if (token.type == IDENTIFIER)
    {
        Node node;
        node.token = token;
        index++;
        return node;
    }

    // If the token is a LEFT_PAREN, then a new operation is starting.
    else if (token.type == LEFT_PAREN)
    {
        Node node;
        index++;

        // Check if the token after LEFT_PAREN is a valid operation. If not, throw an error.
        if (index > (int)tokens.size() - 1 || tokens[index].type != OPERATOR)
        {
            //cout << "should be op" << endl;
            printErrorTwo(tokens[index]);
        } 
        else if(tokens[index].text == "="){
            if(tokens[index + 1].type != IDENTIFIER){
                //cout << "should be ident" << endl;
                printErrorTwo(tokens[index + 1]);
            }
        }
        node.token = tokens[index++];

        // Error checking for an empty operation
        if (tokens[index].type == RIGHT_PAREN)
        {
            //cout << "right paren" << endl;
            printErrorTwo(tokens[index]);
        }

        // While there are more tokens inside the parentheses, recursively create child nodes.
        while (index < (int)tokens.size() - 1 && tokens[index].type != RIGHT_PAREN)
        {
            node.children.push_back(makeTree(tokens, index));
        }
        
        //cout << tokens[index].text << node.token.text << endl;
        if(node.children.size() == 1 && node.token.text == "="){
            printErrorTwo(tokens[index]);
        }
        // Error handling for incorrect expressions (like "(+ 1 2" without the closing parenthesis)
        if (node.children.empty() || (node.children.size() == 1 && node.children[0].token.type != FLOAT && node.children[0].token.type != IDENTIFIER))
        {
            //cout << "no closing paren" << endl;
            printErrorTwo(tokens[index - 1]);
        }
        else if (index < (int)tokens.size() - 1 && tokens[index].type == RIGHT_PAREN)
        {
            index++;
        }
        // If the token is neither a FLOAT, IDENTIFIER nor a LEFT_PAREN, it's unexpected.
        else
        {
            //cout << "unexpected" << endl;
            printErrorTwo(tokens[index]);
        }
        return node;
    }
    else
    {
        //cout << "unexpected" << endl;
        printErrorTwo(token);
    }
    throw 1;
}

//Evaluate
double AST::evaluateAST(std::unordered_map<std::string, double>& variables)
{
    return evaluate(root, variables);
}

// Evaluates the given AST node and returns the result of the original expression while storing the variables.
double AST::evaluate(Node node, std::unordered_map<std::string, double>& variables) const
{
    // If the token is an operator, apply the correct operation.
    double result = 0;
    if(node.token.type == OPERATOR)
    {
        // Iterate over the rest of the children to apply the operation.
        for (size_t i = 0; i < node.children.size(); i++)
        {
            Token opToken = node.token;
            if (opToken.text == "+")
            {
                if(i == 0){
                    result = evaluate(node.children[i], variables);
                }
                else{
                    result += evaluate(node.children[i], variables);
                }
            }
            else if (opToken.text == "-")
            {
                if(i == 0){
                    result = evaluate(node.children[i], variables);
                }
                else{
                    result -= evaluate(node.children[i], variables);
                }
            }
            else if (opToken.text == "*")
            {
                if(i == 0){
                    result = evaluate(node.children[i], variables);
                }
                else{
                    result *= evaluate(node.children[i], variables);
                }
            }
            else if (opToken.text == "/")
            {
                // Check for division by zero.
                if(i == 0){
                    result = evaluate(node.children[i], variables);
                }
                else{
                double denominator = evaluate(node.children[i], variables);
                    if (denominator != 0)
                    {
                        result /= denominator;
                    }
                    else
                    {
                        cout << "Runtime error: division by zero." << endl;
                        exit(3);
                    }
                }
            }
            else if (opToken.text == "=")
            {
                result = evaluate(node.children[node.children.size()-1], variables);
                for(int i = node.children.size()-2; i >= 0; i--){
                    variables[node.children[i].token.text] = result;
                }
            }
            else
            {
                // If the operation is unrecognized, print an error message.
                printErrorTwo(opToken);
                return 2;
            }
        }
    }
    // If the node is a float or an identifier, return the value.
    else if (node.token.type == FLOAT)
    {
        return stod(node.token.text);
    }
    else if(node.token.type == IDENTIFIER){
        //Check for runtime error
        if (variables.count(node.token.text) > 0){
            return variables.at(node.token.text);
        }
        else{
            cout << "Runtime error: unknown identifier " << node.token.text << endl;
            exit(3);
        }
    }
    // If the node does not have any children, throw an error.
    else if (node.children.size() == 0)
    {
        cout << "no children" << endl;
        printErrorTwo(node.token);
        return 2;
    }
    else
    {
        return 2;
    }
    return result;

}

void AST::printInfix() const
{
    if ((root.token.type != FLOAT && root.token.type != IDENTIFIER))
        cout << "(";

    printInfix(root);
    if ((root.token.type != FLOAT && root.token.type != IDENTIFIER))
        cout << ")";

    cout << endl;
}

// Prints the infix notation of a given AST.
void AST::printInfix(const Node node) const
{
    if (node.token.type == FLOAT)
    {
        double val = stod(node.token.text);
        if (val == static_cast<int>(val))
            cout << static_cast<int>(val);
        else
            cout << node.token.text;
    }
    else if (node.token.type == IDENTIFIER)
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
            if (child.token.type != FLOAT && child.token.type != IDENTIFIER)
            {
                cout << "(";
            }
            printInfix(child);
            if (child.token.type != FLOAT && child.token.type != IDENTIFIER)
            {
                cout << ")";
            }
        }
    }
}

// Prints an output 2 error message for a given token
void printErrorTwo(const Token &token)
{
    cout << "Unexpected token at line " << token.lineNumber
         << " column " << token.columnNumber << ": "
         << token.text << endl;
    exit(2);
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
    //text = "(= b c (+ 6 3 4))";
    //text = "(= a b (+ 6 3 4)) \n (+ 1 a 3)";
    //text ="(= a 3)";
    //text = "(+ 4 5 7)";
    //text = "(= a b 3 z)";
    //text = "(=(+x)89)";
    //text = "(= x (+ 3 3) 4) ";

    // lex and then check for lex errors
    tokens = readTokens(text);
    if (tokens.back().text == "error")
    {
        exit(1);
    }

    // set up variables for muti expression parsing
    int index = 0;
    vector<AST> trees;
    std::unordered_map<std::string, double> variables;

    // parse the tokens and put into trees
    while(tokens[index].type != END)
    {
        AST ast(tokens, index);
        trees.push_back(ast);
    }
    //print and evaluate trees
    for(auto tree : trees)
    {
        tree.printInfix();
        cout << tree.evaluateAST(variables) << endl;
    }

    return 0;
}