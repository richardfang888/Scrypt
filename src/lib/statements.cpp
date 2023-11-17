#include "statements.hpp"
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

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
ArrayLiteralNode *makeArrayLiteralNode(const Token &token)
{
    ArrayLiteralNode *aLNode = new ArrayLiteralNode();
    aLNode->token = token;
    return aLNode;
}
// ArrayLookupNode *makeArrayLookupNode(const Token &token)
// {
//     ArrayLookupNode *aLNode = new ArrayLookupNode();
//     aLNode->token = token;
//     return aLNode;
// }
ArrayAssignNode *makeArrayAssignNode(const Token &token)
{
    ArrayAssignNode *aANode = new ArrayAssignNode();
    aANode->token = token;
    return aANode;
}

// Recursively creates an AST from a list of tokens and returns the root node
Node *makeTree(const vector<Token> &tokens, int &index)
{
    bool error = false;
    return parseAll(tokens, index, error);
}

// Parse all tokens in the given vector and return a pointer to the parsed node
Node *parseAll(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    // check first token for keyword
    // parse each statement type given by the keyword parse them, default to plain expression
    if(match(tokens, index, "if"))
    {
        return parseIf(tokens, index, error);
    }
    else if (match(tokens, index, "while"))
    {
        return parseWhile(tokens, index, error);
    }
    else if (match(tokens, index, "print"))
    {
        return parsePrint(tokens, index, error);
    }
    else
    {
        return parseExpression(tokens, index, true, error);
    }
}

// Parses an if statement from a vector of tokens
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
    IENode->condition = parseExpression(tokens, index, false, error);
    // check if the conditionn is a boolean
    index++;

    // check if there is an open bracket
    if (match(tokens, index, "{"))
    {
        // if so skip token
        index++;
    }
    else
    {
        // if not throw error
        printErrorStatement(tokens[index], error);
    }
    // keep parseAlling until close bracket

    while (!match(tokens, index, "}"))
    {
        // each parseAll will return a node that will be pushed into if/esle node's vector
        Node *node = parseAll(tokens, index, error);
        if (node != nullptr)
        {
            IENode->statementsTrue.push_back(node);
        }
        index++;
    }
    // if false

    index ++;

    if (match(tokens, index, "else"))
    {
        IENode->hasElse = true;
        index++;
        // check if there is an open bracket
        if (match(tokens, index, "{"))
        {
            // if so skip token
            index++;
            while (!match(tokens, index, "}"))
            {
                Node *node = parseAll(tokens, index, error);
                if (node != nullptr)
                {
                    IENode->statementsFalse.push_back(node);
                }
                index++;
            }
        }
        else if (match(tokens, index, "if"))
        {
            // run code for else if
            Node *node = parseAll(tokens, index, error);
            if (node != nullptr)
            {
                IENode->statementsFalse.push_back(node);
            }
        }
        else
        {
            // if not throw error
            printErrorStatement(tokens[index], error);
        }
        // keep parseAlling until close bracket
    }
    else
    {
        index--;
        IENode->hasElse = false;
    }
    // return the if/else node
    return IENode;
}

// Parses a while loop from a vector of tokens starting at the given index.
WhileNode *parseWhile(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    // make a new while node
    WhileNode *WNode = makeWhileNode(tokens[index]);
    // skip token
    index++;
    // make the vector
    WNode->condition = parseExpression(tokens, index, false, error);
    index++;

    // check if there is an open bracket
    if (match(tokens, index, "{"))
    {
        // if so skip token
        index++;
    }
    else
    {
        // if not throw error
        printErrorStatement(tokens[index], error);
    }
    // keep parseAlling until close bracket
    while (!match(tokens, index, "}"))
    {
        // each parseAll will return a node that will be pushed into while node's vector
        Node *node = parseAll(tokens, index, error);
        if (node != nullptr)
        {
            WNode->statements.push_back(node);
        }
        index++;
    }
    // return the while node
    return WNode;
}

// Parses a print statement from the given tokens
PrintNode *parsePrint(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    // make a new print node
    PrintNode *PNode = makePrintNode(tokens[index]);
    index++;
    // if/else node's condition = parse expression
    PNode->expression = parseExpression(tokens, index, true, error);
    // return the print node
    return PNode;
}

// Parses an expression from a vector of tokens
Node *parseExpression(const vector<Token> &tokens, int &index, bool checkSemi, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    int startOfExpression = index;
    vector<Token> tokensExpression;

    // checks if the current expression is preceded by a "while" or "if" token
    bool braceCheck = false;

    bool bracketCheck = false;

    bool commaCheck = false;

    if (startOfExpression > 0 && (match(tokens, startOfExpression - 1, "while") || match(tokens, startOfExpression - 1, "if")))
    {
        braceCheck = true;
    }
    if(startOfExpression > 0 && match(tokens, startOfExpression - 1, "["))
    {
        bracketCheck = true;
    }
    if(startOfExpression > 0 && match(tokens, startOfExpression - 1, ","))
    {
        commaCheck = true;
    }
    // Iterate through the tokens to build the expression
    for (size_t x = startOfExpression; x < tokens.size() - 1; x++)
    {
        Token currToken = tokens[x];
        Token nextToken = tokens[x + 1];
        tokensExpression.push_back(currToken);
        if (braceCheck)
        {
            if (nextToken.type == LEFT_BRACE)
            {
                index = x;
                break;
            }
        }
        else if (bracketCheck || commaCheck)
        {
            if (nextToken.type == RIGHT_BRACKET || nextToken.type == COMMA)
            {
                index = x;
                break;
            }
        }
        else
        {
            if (nextToken.lineNumber != currToken.lineNumber || nextToken.type == END)
            {
                index = x;
                break;
            }
        }
    }   
    
    // check that expression ends with semicolon if a print/normal expression
    if (!tokensExpression.empty() && checkSemi)
    {
        // cout << "checkSemi" << endl;
        // for (auto token : tokensExpression)
        // {
        //     cout << token.text;
        // }
        // cout << endl;
        if ((tokensExpression.back().type == END && tokensExpression.size() > 1 && 
                tokensExpression[tokensExpression.size() - 2].text != ";" && 
                tokensExpression[tokensExpression.size() - 2].text != "}") ||
            (tokensExpression.back().type != END && tokensExpression.back().text != ";"))
        {
            // cout << "Error: Expression must end with semicolon" << endl;
            error = true;
            printErrorStatement(tokensExpression.back(), error);
        }
    }
    int assignIndex = 0;
    cout << "tokensExpression size is: " << tokensExpression.size() << endl;
    for(size_t i = 0; i < tokensExpression.size(); i++)
    {
        cout << "Value in index " << i << " is: " << tokensExpression[i].text << " ";
    }
    cout << endl << "Index is: "  << index << endl;
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

// Parses the XOR logic expression from the given tokens
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

// Parses the AND logic expression from the given tokens
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
Node *parseEquality(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseComparison(tokens, index, error);
    while (match(tokens, index, "==") || match(tokens, index, "!="))
    {
        Token opToken = tokens[index++];
        Node *right = parseComparison(tokens, index, error);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse comparison expressions
Node *parseComparison(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseAddSub(tokens, index, error);
    while (match(tokens, index, "<") || match(tokens, index, "<=") ||
           match(tokens, index, ">") || match(tokens, index, ">="))
    {
        Token opToken = tokens[index++];
        Node *right = parseAddSub(tokens, index, error);
        Node *opNode = makeNode(opToken);
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
    if (token.type == FLOAT || token.type == BOOLEAN)
    {
        return makeNode(token);
    }
    else if (token.type == LEFT_BRACKET)
    {
        return parseArrayLiteral(tokens, index, error);
    }
    else if (token.type == IDENTIFIER)
    {
        if(match(tokens, index, "["))
        {
            return parseArrayAssign(tokens, index, error);
        }
        else
        {
            return makeNode(token);
        }
    }
    else if (token.type == TokenType::LEFT_PAREN)
    {
        Node *expression = parseAssignment(tokens, index, error);
        if (!match(tokens, index, ")"))
        {
            // Handle missing closing parenthesis error
            if (index < int(tokens.size()) && !error)
            {
                printErrorStatement(tokens[index], error);
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
        printErrorStatement(token, error);
        return nullptr;
    }
}
ArrayLiteralNode *parseArrayLiteral(const vector<Token> &tokens, int &index, bool &error)
{
    cout << "parsing array literal" << endl;
    if (error)
    {
        return nullptr;
    }
    ArrayLiteralNode *aLNode = makeArrayLiteralNode(tokens[index - 1]);

    cout << "Text of token: " << tokens[index].text << endl;
    // keep parseAlling until close bracket
    while (!match(tokens, index, "]"))
    {
        // each parseAll will return a node that will be pushed into while node's vector
        Node *node = parseExpression(tokens, index, false, error);
        cout << "Text of token made it past parse: " << tokens[index].text << endl;
        if (node != nullptr)
        {
            aLNode->array.push_back(node);
        }
        index ++;
        if(match(tokens, index, "]")){
            break;
        }
        index ++;
    }
    // return the while node
    return aLNode;
}
ArrayAssignNode *parseArrayAssign(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    // set array asign node to the variable before the [
    ArrayAssignNode *aANode = makeArrayAssignNode(tokens[index - 1]);
    //move index to the statement in []
    index++;
    aANode->arrayIndex = parseExpression(tokens, index, false, error);
    // skip the ending ]
    index++;
    return aANode;
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
                printErrorStatement(root->token, error);
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
    if (error)
    {
        return false;
    }
    int count = 0;
    Token lastToken;

    for (const Token &token : tokens)
    {
        if (token.text == "(")
        {
            count++;
        }
        else if (token.text == ")")
        {
            count--;

            if (count < 0)
            {
                // More right parentheses than left parentheses
                printErrorStatement(token, error);
                return false;
            }
        }

        lastToken = token;
    }

    if (count > 0)
    {
        // More left parentheses than right parentheses
        printErrorStatement(lastToken, error);
        return false;
    }

    return true;
}
// Deletes all nodes
void deleteNodeAll(Node *node)
{
    // for if/else node
    if (IfElseNode *iENode = dynamic_cast<IfElseNode *>(node))
    {
        deleteNodeAll(iENode->condition);
        for (Node *child : iENode->statementsTrue)
        {
            deleteNodeAll(child);
        }
        for (Node *child : iENode->statementsFalse)
        {
            deleteNodeAll(child);
        }
        delete iENode;
    }
    // for while node
    else if (WhileNode *wNode = dynamic_cast<WhileNode *>(node))
    {
        deleteNodeAll(wNode->condition);
        for (Node *child : wNode->statements)
        {
            deleteNodeAll(child);
        }
        delete wNode;
    }
    // for print node
    else if (PrintNode *pNode = dynamic_cast<PrintNode *>(node))
    {
        deleteNodeAll(pNode->expression);
        delete pNode;
    }
    else
    {
        for (Node *child : node->children)
        {
            deleteNodeAll(child);
        }
        delete node;
    }
}

void printErrorStatement(const Token &token, bool &error)
{
    error = true;
    cout << "Unexpected token at line " << token.lineNumber
         << " column " << token.columnNumber << ": "
         << token.text << endl;
    exit(2);
}