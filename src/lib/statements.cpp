#include "statements.hpp"
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

// AST(const vector<Token> &tokens, int &index)
// {
//     error = false;
//     root = makeTree(tokens, index);
// }

// ~AST()
// {
//     deleteNode(root);
// }

void deleteNode(Node *node)
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

Node *makeNode(const Token &token)
{
    Node *node = new Node();
    node->token = token;
    return node;
}

WhileNode *makeWhileNode(const Token &token)
{
    WhileNode *wNode = new WhileNode();
    wNode->token = token;
    return wNode;
}
IfElseNode *makeIfElseNode(const Token &token)
{
    IfElseNode *iFNode = new IfElseNode();
    iFNode->token = token;
    return iFNode;
}
PrintNode *makePrintNode(const Token &token)
{
    PrintNode *pNode = new PrintNode();
    pNode->token = token;
    return pNode;
}

// Recursively creates an AST from a list of tokens,
Node *makeTree(const vector<Token> &tokens, int &index)
{
    bool error = false;
    return parseAll(tokens, index, error);
}

Node *parseAll(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    // check first token
    // for each respective token parse them and if not assume assignment
    if(match(tokens, index, "if"))
    {      
        return parseIf(tokens, index, error);
    }
    else if(match(tokens, index, "while"))
    {
        return parseWhile(tokens, index, error);
    }
    else if(match(tokens, index, "print"))
    {
        return parsePrint(tokens, index, error);
    }
    else
    {
        // make a new vector
        return parseExpression(tokens, index, error);
    }
}

IfElseNode *parseIf(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    // make a new if/esle node
    IfElseNode *IENode = makeIfElseNode(tokens[index]); // for testing
    // skip token
    index ++;
    // if/esle node's condition = parse expression 
    IENode->condition = parseExpression(tokens, index, error);
    // check if the conditionn is a boolean
    index ++;

    // check if there is an open bracket
    if(match(tokens, index, "{"))
    {
        // if so skip token
        index ++;
    } else
    {
        // if not throw error
        printError(tokens[index], error);
    }
    // keep parseAlling until close bracket
    while(!match(tokens, index, "}")){
        // each parseAll will return a node that will be pushed into if/esle node's vector
        //cout << "token text: " << tokens[index].text << endl;
        Node *node = parseAll(tokens, index, error);
        if (node != nullptr) {
            IENode->statementsTrue.push_back(node);
        }
        index ++;
    }
    // if false
    index ++;
    //cout << "token text: " << tokens[index].text << endl;
    if(match(tokens, index, "else"))
    {
        //cout << "AM IN ELSEE HERHESRHAERASDFADF" << endl;
        IENode->hasElse = true;
        index ++;
        // check if there is an open bracket
        if(match(tokens, index, "{"))
        {
            // if so skip token
            index ++;
            while(!match(tokens, index, "}")){
            Node *node = parseAll(tokens, index, error);
                if (node != nullptr) {
                    IENode->statementsFalse.push_back(node);
                }
            //cout << "token text: " << tokens[index].text << endl;
            index ++;
            }
        } else if (match(tokens, index, "if")) {
            //cout << "got here at least" << endl;
            Node *node = parseAll(tokens, index, error);
            if (node != nullptr) {
                IENode->statementsFalse.push_back(node);
            }
            //cout << "token text: " << tokens[index].text << endl;     
            
        } else{
            // if not throw error
            printError(tokens[index], error);
        }
        // keep parseAlling until close bracket
    } else
    {
        index --;
        IENode->hasElse = false;
    }
    // return the if/else node
    //cout << "IENode text: " << IENode->token.text << endl;
    //cout << "token text RIGHT BEFORE RETURN: " << tokens[index].text << endl;
    return IENode;
}

WhileNode *parseWhile(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    // make a new if/esle node
    WhileNode *WNode = makeWhileNode(tokens[index]);
    //cout << "WNode text: " << WNode->token.text << endl;
    // skip token
    index ++;
    // if/esle node's condition = parse expression 
    // make the vector
    WNode->condition = parseExpression(tokens, index, error);
    index ++;
    // check if the conditionn is a boolean
    //cout << "got past parseExpression" << endl;
    // check if there is an open bracket
    if(match(tokens, index, "{"))
    {
        // if so skip token
        index ++;
    } else
    {
        // if not throw error
        printError(tokens[index], error);
    }
    // keep parseAlling until close bracket
    //cout << "WHILE Index is " << index << endl;
    while(!match(tokens, index, "}")){
        // each parseAll will return a node that will be pushed into while node's vector
        Node *node = parseAll(tokens, index, error);
        if (node != nullptr) {
            WNode->statements.push_back(node);
        }
        // for(size_t i = 0; i < tokensExpression.size(); i++)
        // {
        //     cout << "value in index " << i << " is: " << tokensExpression[i].text << " ";
        // }
        index ++;

    }
    // return the while node
    //cout << "WNode text: " << WNode->token.text << endl;
    return WNode;
}

PrintNode *parsePrint(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    // make a new print node
    PrintNode *PNode = makePrintNode(tokens[index]); // for testing
    // skip token
    index ++;
    // if/esle node's condition = parse expression 
    PNode->expression = parseExpression(tokens, index, error);
    // return the print node
    return PNode;
}

Node *parseExpression(const vector<Token> &tokens, int &index, bool &error)
{
    //cout << "Gets to PARSE EXPRESSION " << tokens[index].text << endl;
    if (error)
    {
        return nullptr;
    }
    int startOfExpression = index;
    vector<Token> tokensExpression;

    bool braceCheck = false;

    if(startOfExpression > 0 && (match(tokens, startOfExpression - 1, "while")|| match(tokens, startOfExpression - 1, "if")))
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
    // cout << "tokensExpression size is: " << tokensExpression.size() << endl;
    // for(size_t i = 0; i < tokensExpression.size(); i++)
    // {
    //     cout << "value in index " << i << " is: " << tokensExpression[i].text << " ";
    // }
    // cout << endl << "Index is: "  << index << endl;
    return parseAssignment(tokensExpression, assignIndex, error);
}
// Function to parse assignment expressions
Node *parseAssignment(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseLogicOr(tokens, index, error);
    if (match(tokens, index, "="))
    {
        Node *assignNode = makeNode(tokens[index]);
        Node *right = parseAssignment(tokens, ++index, error);
        assignNode->children.push_back(left);
        assignNode->children.push_back(right);
        return assignNode;
    }
    return left;
}
// Function to parse logical expressions
Node *parseLogicOr(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseLogicXor(tokens, index, error);
    while (match(tokens, index, "|"))
    {
        Token opToken = tokens[index++];
        Node *right = parseLogicXor(tokens, index, error);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

Node *parseLogicXor(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseLogicAnd(tokens, index, error);
    while (match(tokens, index, "^"))
    {
        Token opToken = tokens[index++];
        Node *right = parseLogicAnd(tokens, index, error);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

Node *parseLogicAnd(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseEquality(tokens, index, error);
    while (match(tokens, index, "&"))
    {
        Token opToken = tokens[index++];
        Node *right = parseEquality(tokens, index, error);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse equality expressions
Node* parseEquality(const vector<Token>& tokens, int& index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node* left = parseComparison(tokens, index, error);
    while (match(tokens, index, "==") || match(tokens, index, "!="))
    {
        Token opToken = tokens[index++];
        Node* right = parseComparison(tokens, index, error);
        Node* opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse comparison expressions
Node* parseComparison(const vector<Token>& tokens, int& index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node* left = parseAddSub(tokens, index, error);
    while (match(tokens, index, "<") || match(tokens, index, "<=") ||
           match(tokens, index, ">") || match(tokens, index, ">="))
    {
        Token opToken = tokens[index++];
        Node* right = parseAddSub(tokens, index, error);
        Node* opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}


// Function to parse addition and subtraction expressions
Node *parseAddSub(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseMultDivMod(tokens, index, error);
    while (match(tokens, index, "+") || match(tokens, index, "-"))
    {
        Token opToken = tokens[index++];
        Node *right = parseMultDivMod(tokens, index, error);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse multiplication, division, and modulo expressions
Node *parseMultDivMod(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parsePrimary(tokens, index, error);
    while (match(tokens, index, "*") || match(tokens, index, "/") || match(tokens, index, "%"))
    {
        Token opToken = tokens[index++];
        Node *right = parsePrimary(tokens, index, error);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse primary expressions
Node *parsePrimary(const vector<Token> &tokens, int &index, bool &error)
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
        Node *expression = parseAssignment(tokens, index, error);
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
bool match(const vector<Token> &tokens, int index, string expectedType)
{
    if (index >= int(tokens.size()))
    {
        return false;
    }
    return tokens[index].text == expectedType;
}

// Throws runtime error for unknown identifier
bool checkIden(Node *root, unordered_map<string, variant<double, bool>> &variables, bool &error)
{
    if (!root)
    {
        return true;
    }
    if (root->token.text == "=")
    {
        bool check = checkIden(root->children[root->children.size() - 1], variables, error);
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
        if (!checkIden(child, variables, error))
        {
            return false;
        }
    }
    return true;
}

// Throws error for invalid assignment
bool checkVar(Node *root, bool &error)
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
        if (!checkVar(child, error))
        {
            return false;
        }
    }
    return true;
}
// check for parentheses errors
bool checkParen(vector<Token> &tokens, bool &error)
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

// Node *getRoot() const
// {
//     return root;
// }
// Prints a formatted error message for a given token
void printError(const Token &token, bool &error)
{
    error = true;
    cout << "Unexpected token at line " << token.lineNumber
         << " column " << token.columnNumber << ": "
         << token.text << endl;
    exit(2);
}