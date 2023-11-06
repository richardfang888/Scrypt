#include "statements.hpp"
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

AST::AST(const vector<Token> &tokens, int &index)
{
    error = false;
    int index = index;
    root = makeTree(tokens, index);
}

AST::~AST()
{
    deleteNode(root);
}

void AST::deleteNode(Node *node)
{
    if (node)
    {
        for (Node *child : node->children)
        {
            deleteNode(child);
        }
        delete node;
    }
}

Node *AST::makeNode(const Token &token)
{
    Node *node = new Node();
    node->token = token;
    return node;
}

// Recursively creates an AST from a list of tokens,
Node *AST::makeTree(const vector<Token> &tokens, int &index)
{
    return parseAll(tokens, index);
}

Node *AST::parseAll(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    // check first token
    // for each respective token parse them and if not assume assignment
    if(match(tokens, index, "if"))
    {
        return parseIf(tokens, index);
    }
    else if(match(tokens, index, "while"))
    {
        return parseWhile(tokens, index);
    }
    else if(match(tokens, index, "print"))
    {
        return parsePrint(tokens, index);
    }
    else
    {
        // make a new vector
        return parseExpression(tokens, index);
    }
}

Node *AST::parseIf(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    // make a new if/esle node
    IfElseNode *IENode;
    // skip token
    index ++;
    // if/esle node's condition = parse expression 
    IENode->condition = parseExpression(tokens, index);
    // check if the conditionn is a boolean

    // check if there is an open bracket
    if(match(tokens, index, "{"))
    {
        // if so skip token
        index ++;
    } else
    {
        // if not throw error
        printError(tokens[index], error);
        return nullptr;
    }
    // keep parseAlling until close bracket
    while(!match(tokens, index, "}")){
        // each parseAll will return a node that will be pushed into if/esle node's vector
        Node *node = parseAll(tokens, index);
        if (node != nullptr) {
            IENode->statementsTrue.push_back(node);
        }
    }
    // if false
    if(match(tokens, index, "else"))
    {
        IENode->hasElse = true;
        // check if there is an open bracket
        if(match(tokens, index, "{"))
        {
            // if so skip token
            index ++;
        } else
        {
            // if not throw error
            printError(tokens[index], error);
            return nullptr;
        }
        // keep parseAlling until close bracket
        while(!match(tokens, index, "}")){
            Node *node = parseAll(tokens, index);
            if (node != nullptr) {
                IENode->statementsFalse.push_back(node);
            }
        }
    } else
    {
        IENode->hasElse = false;
    }
    // return the if/else node
    return IENode;
}

Node *AST::parseWhile(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    // make a new if/esle node
    WhileNode *WNode;
    // skip token
    index ++;
    // if/esle node's condition = parse expression 
    // make the vector
    WNode->condition = parseExpression(tokens, index);
    // check if the conditionn is a boolean

    // check if there is an open bracket
    if(match(tokens, index, "{"))
    {
        // if so skip token
        index ++;
    } else
    {
        // if not throw error
        printError(tokens[index], error);
        return nullptr;
    }
    // keep parseAlling until close bracket
    while(!match(tokens, index, "}")){
        // each parseAll will return a node that will be pushed into while node's vector
        Node *node = parseAll(tokens, index);
        if (node != nullptr) {
            WNode->statements.push_back(node);
        }
    }
    // return the while node
    return WNode;
}

Node *AST::parsePrint(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    // make a new print node
    PrintNode *PNode;
    // skip token
    index ++;
    // if/esle node's condition = parse expression 
    PNode->expression = parseExpression(tokens, index);

    // return the print node
    return PNode;
}

Node *AST::parseExpression(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    int startOfExpression = index;
    vector<Token> tokensExpression;

    bool braceCheck = false;

    if(startOfExpression > 0 && (match(tokens, startOfExpression - 1, "while")|| match(tokens, startOfExpression, "if")))
    {
        braceCheck = true;
    }

    for(size_t x = startOfExpression; x < tokens.size() - 1; x++)
    {
        Token currToken = tokens[x];
        Token nextToken = tokens[x + 1];
        tokensExpression.push_back(currToken);

        if(braceCheck)
        {
            if(nextToken.type == LEFT_BRACE)
            {
                index = x;
                break;
            }
        }
        else
        {
            if(nextToken.lineNumber != currToken.lineNumber || nextToken.type == END)
            {
                index = x;
                break;
            }
        }
    }
    int assignIndex = 0;
    return parseAssignment(tokensExpression, assignIndex);
}
// Function to parse assignment expressions
Node *AST::parseAssignment(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseLogicOr(tokens, index);
    if (match(tokens, index, "="))
    {
        Node *assignNode = makeNode(tokens[index]);
        Node *right = parseAssignment(tokens, ++index);
        assignNode->children.push_back(left);
        assignNode->children.push_back(right);
        return assignNode;
    }
    return left;
}

// Function to parse logical expressions
Node *AST::parseLogicOr(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseLogicXor(tokens, index);
    while (match(tokens, index, "|"))
    {
        Token opToken = tokens[index++];
        Node *right = parseLogicXor(tokens, index);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

Node *AST::parseLogicXor(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseLogicAnd(tokens, index);
    while (match(tokens, index, "^"))
    {
        Token opToken = tokens[index++];
        Node *right = parseLogicAnd(tokens, index);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

Node *AST::parseLogicAnd(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseEquality(tokens, index);
    while (match(tokens, index, "&"))
    {
        Token opToken = tokens[index++];
        Node *right = parseEquality(tokens, index);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse equality expressions
Node* AST::parseEquality(const vector<Token>& tokens, int& index)
{
    if (error)
    {
        return nullptr;
    }
    Node* left = parseComparison(tokens, index);
    while (match(tokens, index, "==") || match(tokens, index, "!="))
    {
        Token opToken = tokens[index++];
        Node* right = parseComparison(tokens, index);
        Node* opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse comparison expressions
Node* AST::parseComparison(const vector<Token>& tokens, int& index)
{
    if (error)
    {
        return nullptr;
    }
    Node* left = parseAddSub(tokens, index);
    while (match(tokens, index, "<") || match(tokens, index, "<=") ||
           match(tokens, index, ">") || match(tokens, index, ">="))
    {
        Token opToken = tokens[index++];
        Node* right = parseAddSub(tokens, index);
        Node* opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}


// Function to parse addition and subtraction expressions
Node *AST::parseAddSub(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseMultDivMod(tokens, index);
    while (match(tokens, index, "+") || match(tokens, index, "-"))
    {
        Token opToken = tokens[index++];
        Node *right = parseMultDivMod(tokens, index);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse multiplication, division, and modulo expressions
Node *AST::parseMultDivMod(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parsePrimary(tokens, index);
    while (match(tokens, index, "*") || match(tokens, index, "/") || match(tokens, index, "%"))
    {
        Token opToken = tokens[index++];
        Node *right = parsePrimary(tokens, index);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse primary expressions
Node *AST::parsePrimary(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    Token token = tokens[index++];
    if (token.type == FLOAT || token.type == IDENTIFIER || token.type == BOOLEAN)
    {
        return makeNode(token);
    }
    else if (token.type == TokenType::LEFT_PAREN)
    {
        Node *expression = parseAssignment(tokens, index);
        if (!match(tokens, index, ")"))
        {
            // Handle missing closing parenthesis error
            if (index < int(tokens.size()) && !error)
            {
                // cout << "parenthesis" << endl;
                printError(tokens[index], error);
            }
            deleteNode(expression);
            return nullptr;
        }
        ++index; // Increment index to skip the closing parenthesis
        return expression;
    }
    else
    {
        // Handle unexpected token error
        // cout << "primary" << endl;
        printError(token, error);
        return nullptr;
    }
}

// Utility function to check if the current token matches the expected token type
bool AST::match(const vector<Token> &tokens, int index, string expectedType)
{
    if (index >= int(tokens.size()))
    {
        return false;
    }
    return tokens[index].text == expectedType;
}

// Throws runtime error for unknown identifier
bool AST::checkIden(Node *root, unordered_map<string, variant<double, bool>> &variables)
{
    if (!root)
    {
        return true;
    }
    if (root->token.text == "=")
    {
        bool check = checkIden(root->children[root->children.size() - 1], variables);
        return check;
    }
    // If the node is an IDENTIFIER token, check if it exists in the variables map
    else if (root->token.type == IDENTIFIER)
    {
        string identifierText = root->token.text;

        // Check if the identifier exists in the variables unordered_map
        auto iter = variables.find(identifierText);
        if (iter == variables.end())
        {
            // Handle error: Unknown identifier
            error = true;
            cout << "Runtime error: unknown identifier " + identifierText << endl;
            return false;
        }
    }
    // Recursively check the children nodes
    for (Node *child : root->children)
    {
        if (!checkIden(child, variables))
        {
            return false;
        }
    }
    return true;
}

// Throws error for invalid assignment
bool AST::checkVar(Node *root)
{
    if (!root)
    {
        return true;
    }
    if (root->token.text == "=")
    {
        for (int i = int(root->children.size() - 2); i >= 0; i--)
        {
            if (error)
            {
                break;
            }
            if (root->children[i]->token.type != IDENTIFIER)
            {
                // invalid assignment error
                printError(root->token, error);
                return false;
            }
        }
    }
    // Recursively check the children nodes
    for (Node *child : root->children)
    {
        if (error)
        {
            break;
        }
        if (!checkVar(child))
        {
            return false;
        }
    }
    return true;
}

// check for parentheses errors
bool AST::checkParen(vector<Token> &tokens)
{
    if (error) {
        return false;
    }
    int count = 0;
    Token lastToken;
    
    for (const Token &token : tokens) {
        if (token.text == "(") {
            count++;
        } else if (token.text == ")") {
            count--;
            
            if (count < 0) {
                // More right parentheses than left parentheses
                printError(token, error);
                return false;
            }
        }
        
        lastToken = token;
    }
    
    if (count > 0) {
        // More left parentheses than right parentheses
        printError(lastToken, error);
        return false;
    }
    
    return true;
}

Node *AST::getRoot() const
{
    return root;
}

// Prints a formatted error message for a given token
void printError(const Token &token, bool &error)
{
    error = true;
    cout << "Unexpected token at line " << token.lineNumber
         << " column " << token.columnNumber << ": "
         << token.text << endl;
}