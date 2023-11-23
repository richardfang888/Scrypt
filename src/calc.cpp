#include "lib/calc.hpp"
#include <iostream>
#include <limits>
#include <cmath>
#include <stack>
#include <iomanip>

using namespace std;

void deleteNodeCalc(Node *node)
{
    if (node != nullptr)
    {
        if (ArrayLiteralNode *aLNode = dynamic_cast<ArrayLiteralNode *>(node))
        {
            for (Node *child : aLNode->array)
            {
                deleteNodeCalc(child);
            }
            delete aLNode;
        }
        // for array assign node
        else if (ArrayAssignNode *aANode = dynamic_cast<ArrayAssignNode *>(node))
        {
            deleteNodeCalc(aANode->expression);
            deleteNodeCalc(aANode->arrayIndex);
            delete aANode;
        }
        else if (ReturnNode *rNode = dynamic_cast<ReturnNode *>(node))
        {
            if (rNode->expression)
            {
                deleteNodeCalc(rNode->expression);
            }
            delete rNode;
        }
        // for function definition
        else if (FunctDefNode *fNode = dynamic_cast<FunctDefNode *>(node))
        {
            for (Node *child : fNode->statements)
            {
                deleteNodeCalc(child);
            }
            delete fNode;
        }
        // for function call
        else if (FunctCallNode *fNode = dynamic_cast<FunctCallNode *>(node))
        {
            for (Node *child : fNode->arguments)
            {
                deleteNodeCalc(child);
            }
            delete fNode;
        }
        else
        {
            for (Node *child : node->children)
            {
                deleteNodeCalc(child);
            }
            delete node;
        }
        
    }
}

Node *makeNodeCalc(const Token &token)
{
    Node *node = new Node();
    node->token = token;
    return node;
}
ArrayLiteralNode *makeArrayLiteralNodeCalc(const Token &token)
{
    ArrayLiteralNode *aLNode = new ArrayLiteralNode();
    aLNode->token = token;
    return aLNode;
}

ArrayAssignNode *makeArrayAssignNodeCalc(const Token &token)
{
    ArrayAssignNode *aANode = new ArrayAssignNode();
    aANode->token = token;
    return aANode;
}

FunctCallNode *makeFunctCallNodeCalc(const Token &token)
{
    FunctCallNode *fcNode = new FunctCallNode();
    fcNode->functname = token;
    return fcNode;
}

// Recursively creates an AST from a list of tokens
Node *makeTreeCalc(const vector<Token> &tokens, int &index, bool &error)
{
    return parseAssignmentCalc(tokens, index, error);
}

// Parses a function call from the given tokens
FunctCallNode *parseFunctCallCalc(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    // make a new function call node
    FunctCallNode *FNode = makeFunctCallNodeCalc(tokens[index - 1]);
    FNode->functname = tokens[index - 1];
    // cout << "make funct call node for " << FNode->functname.text << endl;
    index++;
    if (matchCalc(tokens, index, ","))
    {
        error = true;
        printErrorCalc(tokens[index], error);
    }
    while (!matchCalc(tokens, index, ")"))
    {
        Node *node = parseExpressionCalc(tokens, index, error);
        // cout << "node: " << node->token.text << endl;
        if (node != nullptr)
        {
            FNode->arguments.push_back(node);
        }
        if(matchCalc(tokens, index, ")")){
            break;
        }
        index++;
        // if(matchCalc(tokens, index - 1, ",")){
        //     continue;
        // }
        // cout << "here " << tokens[index].text << endl;
        if(matchCalc(tokens, index, ")")){
            break;
        }
        index++;
        // cout << "here 2" << tokens[index].text << endl;
    }
    index++;
    // cout << "finished parsing function call" << endl;
    return FNode;
}

Node *parseExpressionCalc(const vector<Token> &tokens, int &index, bool &error)
{
    // cout << "index and curr token text: " << index << ", " << tokens[index].text << endl;
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
    bool parenCheck = false;

    if (startOfExpression > 0 && (matchCalc(tokens, startOfExpression - 1, "while") 
        || matchCalc(tokens, startOfExpression - 1, "if") || matchCalc(tokens, startOfExpression - 1, "def")))
    {
        braceCheck = true;
    }
    if (startOfExpression > 0 && matchCalc(tokens, startOfExpression - 1, "["))
    {
        bracketCheck = true;
    }
    if (startOfExpression > 0 && matchCalc(tokens, startOfExpression - 1, ","))
    {
        commaCheck = true;
    }
    if (startOfExpression > 1 && matchCalc(tokens, startOfExpression - 1, "(") && tokens[startOfExpression - 2].type == IDENTIFIER) 
    {
        parenCheck = true;
    }
    int nested = 0;
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
        else if (bracketCheck || commaCheck || parenCheck)
        {
            if (currToken.type == LEFT_PAREN || currToken.type == LEFT_BRACKET)
            {
                nested++;
            }
            if (nested == 0 && (nextToken.type == RIGHT_BRACKET || nextToken.type == COMMA || nextToken.type == RIGHT_PAREN))
            {
                index = x;
                break;
            }
            if (nextToken.type == RIGHT_PAREN || nextToken.type == RIGHT_BRACKET)
            {
                nested--;
            }
        }
        else if (matchCalc(tokens, startOfExpression, "["))
        {
            if (currToken.type == SEMICOLON)
            {
                index = x;
                break;
            }
        }
        else
        {
            if (currToken.length != nextToken.lineNumber  || nextToken.type == END || currToken.text == ";")
            {
                index = x;
                break;
            }
        }
    }
    int assignIndex = 0;
    return parseAssignmentCalc(tokensExpression, assignIndex, error);
}

// Function to parse assignment expressions
Node *parseAssignmentCalc(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseLogicOrCalc(tokens, index, error);
    if (matchCalc(tokens, index, "="))
    {
        Node *assignNode = makeNodeCalc(tokens[index]);
        Node *right = parseAssignmentCalc(tokens, ++index, error);
        assignNode->children.push_back(left);
        assignNode->children.push_back(right);
        return assignNode;
    }
    return left;
}

// Function to parse logical expressions
Node *parseLogicOrCalc(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseLogicXorCalc(tokens, index, error);
    while (matchCalc(tokens, index, "|"))
    {
        Token opToken = tokens[index++];
        Node *right = parseLogicXorCalc(tokens, index, error);
        Node *opNode = makeNodeCalc(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Parses the XOR logic expression from the given tokens
Node *parseLogicXorCalc(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseLogicAndCalc(tokens, index, error);
    while (matchCalc(tokens, index, "^"))
    {
        Token opToken = tokens[index++];
        Node *right = parseLogicAndCalc(tokens, index, error);
        Node *opNode = makeNodeCalc(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Parses the AND logic expression from the given tokens
Node *parseLogicAndCalc(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseEqualityCalc(tokens, index, error);
    while (matchCalc(tokens, index, "&"))
    {
        Token opToken = tokens[index++];
        Node *right = parseEqualityCalc(tokens, index, error);
        Node *opNode = makeNodeCalc(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse equality expressions
Node *parseEqualityCalc(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseComparisonCalc(tokens, index, error);
    bool isArray = false;
    if (matchCalc(tokens, index, "]")) {
        isArray = true;
    }
    if (isArray) {
        index ++;
    }
    while (matchCalc(tokens, index, "==") || matchCalc(tokens, index, "!="))
    {
        Token opToken = tokens[index++];
        Node *right = parseComparisonCalc(tokens, index, error);
        Node *opNode = makeNodeCalc(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    if (isArray) {
        index --;
    }
    return left;
}

// Function to parse comparison expressions
Node *parseComparisonCalc(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseAddSubCalc(tokens, index, error);
    while (matchCalc(tokens, index, "<") || matchCalc(tokens, index, "<=") ||
           matchCalc(tokens, index, ">") || matchCalc(tokens, index, ">="))
    {
        Token opToken = tokens[index++];
        Node *right = parseAddSubCalc(tokens, index, error);
        Node *opNode = makeNodeCalc(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse addition and subtraction expressions
Node *parseAddSubCalc(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseMultDivModCalc(tokens, index, error);
    while (matchCalc(tokens, index, "+") || matchCalc(tokens, index, "-"))
    {
        Token opToken = tokens[index++];
        Node *right = parseMultDivModCalc(tokens, index, error);
        Node *opNode = makeNodeCalc(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse multiplication, division, and modulo expressions
Node *parseMultDivModCalc(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parsePrimaryCalc(tokens, index, error);
    while (matchCalc(tokens, index, "*") || matchCalc(tokens, index, "/") || matchCalc(tokens, index, "%"))
    {
        Token opToken = tokens[index++];
        Node *right = parsePrimaryCalc(tokens, index, error);
        Node *opNode = makeNodeCalc(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

// Function to parse primary expressions
Node *parsePrimaryCalc(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    Token token = tokens[index++];
    if (token.type == FLOAT || token.type == NULLVAL)
    {
        return makeNodeCalc(token);
    }
    else if (token.type == BOOLEAN) {
        // cout << "normal identifier" << endl;
        if (matchCalc(tokens, index, "["))
        {
            Node *identifier = makeNodeCalc(token);
            ArrayAssignNode *arrayAssign = parseArrayAssignCalc(tokens, index, error);
            arrayAssign->expression = identifier;
            return arrayAssign;
        }
        else {
            return makeNodeCalc(token);
        }
    }
    else if (token.type == LEFT_BRACKET)
    {
        // cout << "no shot we are making lit nodes"   << endl;
        ArrayLiteralNode *array = parseArrayLiteralCalc(tokens, index, error);
        // cout << "current token text: " << tokens[index].text << endl;
        if (matchCalc(tokens, index + 1, "["))
        {
            index++;
            // cout << "parsing lit going into assign" << endl;
            ArrayAssignNode *arrayAssign = parseArrayAssignCalc(tokens, index, error);
            arrayAssign->expression = array;
            return arrayAssign;
        }
        else
        {
            return array;
        }
    }
    else if (token.type == IDENTIFIER)
    {
        // cout << "normal identifier" << endl;
        if (matchCalc(tokens, index, "["))
        {
            Node *identifier = makeNodeCalc(token);
            ArrayAssignNode *arrayAssign = parseArrayAssignCalc(tokens, index, error);
            arrayAssign->expression = identifier;
            return arrayAssign;
        }
        else if (matchCalc(tokens, index, "("))
        {
            // cout << "Parsing function call: " << token.text << endl;
            return parseFunctCallCalc(tokens, index, error);
        }
        else
        {
            return makeNodeCalc(token);
        }
    }
    else if (token.type == TokenType::LEFT_PAREN)
    {
        Node *expression = parseAssignmentCalc(tokens, index, error);
        if (!matchCalc(tokens, index, ")"))
        {
            // Handle missing closing parenthesis error
            if (index < int(tokens.size()) && !error)
            {
                printErrorCalc(tokens[index], error);
            }
            deleteNodeCalc(expression);
            return nullptr;
        }
        ++index; // Increment index to skip the closing parenthesis
        return expression;
    }
    else
    {
        // Handle unexpected token error
        printErrorCalc(token, error);
        return nullptr;
    }
}

ArrayLiteralNode *parseArrayLiteralCalc(const vector<Token> &tokens, int &index, bool &error)
{
    // cout << "parsing array literal" << endl;
    if (error)
    {
        return nullptr;
    }
    ArrayLiteralNode *aLNode = makeArrayLiteralNodeCalc(tokens[index - 1]);

    // cout << "Text of token: " << tokens[index].text << endl;
    //  keep parsing until close bracket
    while (!matchCalc(tokens, index, "]"))
    {
        // each parse will return a node that will be pushed into while node's vector
        Node *node = parseExpressionCalc(tokens, index, error);
        // cout << "Text of token made it past parse: " << tokens[index].text << endl;
        if (node != nullptr)
        {
            aLNode->array.push_back(node);
        }
        index++;
        if (matchCalc(tokens, index, "]"))
        {
            break;
        }
        index++;
    }
    // return the while node
    return aLNode;
}
ArrayAssignNode *parseArrayAssignCalc(const vector<Token> &tokens, int &index, bool &error)
{
    if (error)
    {
        return nullptr;
    }
    // set array asign node to the variable before the [
    ArrayAssignNode *aANode = makeArrayAssignNodeCalc(tokens[index - 1]);
    // move index to the statement in []
    index++;
    aANode->arrayIndex = parseExpressionCalc(tokens, index, error);
    // skip the ending ]
    index += 2;
    return aANode;
}

// Utility function to check if the current token matches the expected token type
bool matchCalc(const vector<Token> &tokens, int index, string expectedType)
{
    if (index >= int(tokens.size()))
    {
        return false;
    }
    return tokens[index].text == expectedType;
}

// Throws runtime error for unknown identifier
bool checkIdenCalc(Node *root, unordered_map<string, Value> &variables, bool &error)
{
    if (!root)
    {
        return true;
    }
    if (root->token.text == "=")
    {
        bool check = checkIdenCalc(root->children[root->children.size() - 1], variables, error);
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
        if (!checkIdenCalc(child, variables, error))
        {
            return false;
        }
    }
    return true;
}


// check for parentheses errors
bool checkParenCalc(vector<Token> &tokens, bool &error)
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
                printErrorCalc(token, error);
                return false;
            }
        }

        lastToken = token;
    }

    if (count > 0)
    {
        // More left parentheses than right parentheses
        printErrorCalc(lastToken, error);
        return false;
    }

    return true;
}

Value returnVal = Value{numeric_limits<double>::quiet_NaN()};

Value evaluateAllCalc(Node *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    if (ReturnNode *rNode = dynamic_cast<ReturnNode *>(node))
    {
        // Node is a ReturnNode
        return evaluateReturnCalc(rNode, variables, error, inFunct);
    }
    else if (FunctDefNode *fdNode = dynamic_cast<FunctDefNode *>(node))
    {
        // Node is a FunctDefNode
        return evaluateFunctDefCalc(fdNode, variables, error, inFunct);
    }
    else if (FunctCallNode *fcNode = dynamic_cast<FunctCallNode *>(node))
    {
        // Node is a FunctCallNode
        return evaluateFunctCallCalc(fcNode, variables, error, inFunct);
    }
    else if (ArrayLiteralNode *alNode = dynamic_cast<ArrayLiteralNode *>(node))
    {
        // Node is an ArrayLiteralNode
        return evaluateArrayLiteralCalc(alNode, variables, error, inFunct);
    }
    else if (ArrayAssignNode *alNode = dynamic_cast<ArrayAssignNode *>(node))
    {
        // Node is an ArrayAssignNode
        return evaluateArrayAssignCalc(alNode, variables, error, inFunct, false);
    }
    else
    {
        // Node is a normal Node
        return evaluateExpressionCalc(node, variables, error, inFunct);
    }
    return Value{numeric_limits<double>::quiet_NaN()};
}


// helper function for evaluate print
void printValueCalc(Value value) 
{
    if (holds_alternative<double>(value) && !isnan(get<double>(value)))
    {
        cout << get<double>(value);
    }
    else if (holds_alternative<bool>(value))
    {
        if (get<bool>(value)) {
            cout << "true";
        }
        else {
            cout << "false";
        }
    }
    else if (value.index() == 2)
    {
        cout << "null";
    }
    else if (value.index() == 3)
    {
        auto vec = *get<shared_ptr<vector<Value>>>(value);
        cout << "[";
        for (size_t i = 0; i < vec.size(); i++)
        {
            if (i == vec.size() - 1)
            {
                printValueCalc(vec[i]);
            }
            else
            {
                printValueCalc(vec[i]);
                cout << ", ";
            }
        }
        cout << "]";
    }
    else {
        cout << "cannot print value";
    }
}

// Evaluates a return statement.
Value evaluateReturnCalc(ReturnNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    // cout << "eval return" << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    if (!inFunct) 
    {
        error = true;
        cout << "Runtime error: unexpected return." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    if (!node->expression)
    {
        return Value{nullptr};
    }
    Value result = evaluateExpressionCalc(node->expression, variables, error, inFunct);
    returnVal = result;

    // set the value of the function call to return
    return result;
}

// Evaluates a function definition.
Value evaluateFunctDefCalc(FunctDefNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    // cout << "Function " << node->functname.text << " defined." << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    // add function to defined functions
    Function func;
    func.function = node;
    func.functVariables = variables;
    variables[node->functname.text] = Value{make_shared<Function>(func)};

    // debugging scope
    // cout << "function def scope: " << endl;
    // for (auto const& x : func.functVariables) {
    //     cout << x.first << ": ";
    //     printValue(x.second);
    //     cout << endl;
    // }

    // dummy return
    inFunct = inFunct;
    return Value{numeric_limits<double>::quiet_NaN()};
}

// Evaluates a function call.
Value evaluateFunctCallCalc(FunctCallNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    // cout << "Function " << node->functname.text << " called." << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }

    if (node->functname.text == "len" || node->functname.text == "push" || node->functname.text == "pop")
    {
        return evaluateUtilityFunctCalc(node, variables, error, inFunct);
    }

    // check if function exists
    auto iter = variables.find(node->functname.text);
    if (iter == variables.end()) 
    {
        // cout << "can't find: " << node->functname.text << endl;
        error = true;
        cout << "Runtime error: not a function." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    Value functionValue = iter->second;
    // check if function is a function type
    if (!holds_alternative<shared_ptr<Function>>(functionValue))
    {
        error = true;
        cout << "Runtime error: not a function." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    shared_ptr<Function> function = get<shared_ptr<Function>>(functionValue);
    
    // check if number of arguments matches
    if (node->arguments.size() != function->function->params.size()) 
    {
        error = true;
        cout << "Runtime error: incorrect argument count." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    // evaluate arguments
    vector<Value> evaluatedArguments;
    for (Node *argument : node->arguments)
    {
        Value result = evaluateAllCalc(argument, variables, error, inFunct);
        evaluatedArguments.push_back(result);
        inFunct = true;
    }

    // create new scope for function call
    unordered_map<string, Value> newVariables = function->functVariables;
    for (size_t i = 0; i < function->function->params.size(); i++)
    {
        newVariables[function->function->params[i]] = evaluatedArguments[i];
    }

    for (const auto& entry : variables) {
        if (entry.second.index() == 4) {
            newVariables[entry.first] = entry.second;
        }
    }

    inFunct = true;
    // Evaluate the statements within the function scope
    for (Node *statement : function->function->statements)
    {
        Value result = evaluateAllCalc(statement, newVariables, error, inFunct);
        if (error) {
            // Handle errors that occurred during evaluation
            return Value{numeric_limits<double>::quiet_NaN()};
        }
        if (returnVal.index() != 0 || !isnan(get<double>(returnVal))) {
            Value functReturn = returnVal;
            returnVal = Value{numeric_limits<double>::quiet_NaN()};
            return functReturn;
        }
    }
    inFunct = false;
    return Value{nullptr};
}

Value evaluateUtilityFunctCalc(FunctCallNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    bool argCountError = false;
    if (node->functname.text == "len")
    {
        // cout << "evaluate len" << endl;
        if (node->arguments.size() != 1)
        {
            argCountError = true;
        }
        else
        {
            Value array = evaluateAllCalc(node->arguments[0], variables, error, inFunct);
            return lenCalc(array, error);
        }
    }
    else if (node->functname.text == "push")
    {
        // cout << "evaluate push" << endl;
        if (node->arguments.size() != 2)
        {
            argCountError = true;
        }
        else
        {
            Value array = evaluateAllCalc(node->arguments[0], variables, error, inFunct);
            Value value = evaluateAllCalc(node->arguments[1], variables, error, inFunct);
            return pushCalc(array, value, error);
        }
    }
    else
    {
        // cout << "evaluate pop" << endl;
        if (node->arguments.size() != 1)
        {
            argCountError = true;
        }
        else
        {
            Value array = evaluateAllCalc(node->arguments[0], variables, error, inFunct);
            return popCalc(array, error);
        }
    }

    if (argCountError)
    {
        error = true;
        cout << "Runtime error: incorrect argument count." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    return Value{nullptr};
}


// Evaluates an array literal
Value evaluateArrayLiteralCalc(ArrayLiteralNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct) 
{
    // cout << "eval array literal" << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    vector<Value> evaluatedArray;
    for (Node *element : node->array)
    {
        evaluatedArray.push_back(evaluateAllCalc(element, variables, error, inFunct));
    }
    return Value{make_shared<vector<Value>>(evaluatedArray)};
}

Value evaluateArrayAssignCalc(ArrayAssignNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct, bool setValue)
{
    // cout << "eval array assign" << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    Value arrayIndexValue = evaluateAllCalc(node->arrayIndex, variables, error, inFunct);
    double intPart;
    if (arrayIndexValue.index() != 0)
    {
        error = true;
        cout << "Runtime error: index is not a number." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    else if (modf(get<double>(arrayIndexValue), &intPart) != 0)
    {
        error = true;
        cout << "Runtime error: index is not an integer." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    Value arrayExpression = evaluateAllCalc(node->expression, variables, error, inFunct);
    if (arrayExpression.index() != 3)
    {
        error = true;
        cout << "Runtime error: not an array." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    auto array = *get<shared_ptr<vector<Value>>>(arrayExpression);
    if (int(get<double>(arrayIndexValue)) >= int(array.size()) || int(get<double>(arrayIndexValue)) < 0)
    {
        error = true;
        cout << "Runtime error: index out of bounds." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    if (setValue) 
    {
        return arrayExpression;
    }
    auto result = array[int(get<double>(arrayIndexValue))];
    return result;
}

// Evaluates an expression given the root of the expression's AST.
Value evaluateExpressionCalc(Node *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    // cout << "eval expression, root is " << node->token.text << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    // If the node holds a FLOAT token, simply return its value.
    if (node->token.type == FLOAT)
    {
        Value result{stod(node->token.text)};
        // cout << "returning float " << get<double>(result) << " ";
        return result;
    }
    if (FunctCallNode *fcNode = dynamic_cast<FunctCallNode *>(node))
    {
        // Node is a FunctCallNode
        return evaluateFunctCallCalc(fcNode, variables, error, inFunct);
    }
    if (ArrayLiteralNode *aLNode = dynamic_cast<ArrayLiteralNode *>(node))
    {
        // Node is an ArrayLiteralNode
        return evaluateArrayLiteralCalc(aLNode, variables, error, inFunct);
    }
    if (ArrayAssignNode *aANode = dynamic_cast<ArrayAssignNode *>(node))
    {
        // Node is an ArrayAssignNode
        return evaluateArrayAssignCalc(aANode, variables, error, inFunct, false);
    }
    // If the node holds a BOOLEAN token, simply return its value.
    if (node->token.type == BOOLEAN)
    {
        if (node->token.text == "true")
        {
            return Value{true};
        }
        else
        {
            return Value{false};
        }
    }
    if (node->token.type == NULLVAL)
    {
        return Value{nullptr};
    }
    // If the node is an IDENTIFIER token, return its value if it exists in the variables map
    // NOTE: This only runs if an IDENTIFIER is found not during assignment
    if (node->token.type == IDENTIFIER)
    {
        string identifierText = node->token.text;

        // Check if the identifier exists in the variables unordered_map
        auto iter = variables.find(identifierText);
        if (iter != variables.end())
        {
            // Return the value of the identifier
            return iter->second;
        }
        else
        {
            // Handle error: Unknown identifier
            error = true;
            cout << "Runtime error: unknown identifier " + identifierText << endl;
            return Value{numeric_limits<double>::quiet_NaN()};
        }
    }
    // Node is an operator but has no children
    else if (node->children.size() == 0)
    {
        // cout << node->token.text << endl;
        printErrorCalc(node->token, error);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    // Node is assignment operator
    else if (node->token.text == "=")
    {
        Value result = evaluateExpressionCalc(node->children[node->children.size() - 1], variables, error, inFunct);
        for (int i = int(node->children.size() - 2); i >= 0; i--)
        {
            if (ArrayAssignNode *aANode = dynamic_cast<ArrayAssignNode *>(node->children[i]))
            {
                // cout << "array assign found" << endl;
                Value array = evaluateArrayAssignCalc(aANode, variables, error, inFunct, true);
                if (error) {
                    return Value{numeric_limits<double>::quiet_NaN()};
                }
                Value index = evaluateAllCalc(aANode->arrayIndex, variables, error, inFunct);
                // printValue(array);
                // printValue(index);
                shared_ptr<vector<Value>> arrayPtr = get<shared_ptr<vector<Value>>>(array);
                (*arrayPtr)[int(get<double>(index))] = result;
            }
            else if (node->children[i]->token.type != IDENTIFIER)
            {
                error = true;
                cout << "Runtime error: invalid assignee." << endl;

                return Value{numeric_limits<double>::quiet_NaN()};
            }
            else
            {
                variables[node->children[i]->token.text] = result;
            }
            // cout << variables[node->children[i]->token.text].index() << endl;
        }
        return result;
    }
    // Node is a non-assignment operator
    else if (node->token.type == OPERATOR)
    {
        Value result = evaluateExpressionCalc(node->children[0], variables, error, inFunct);
        if (holds_alternative<bool>(result))
        {
            // runtime error
            error = true;
            cout << "Runtime error: invalid operand type." << endl;
            return  Value{numeric_limits<double>::quiet_NaN()};
        }
        // Iterate over the rest of the children to apply the operation.
        for (size_t i = 1; i < node->children.size(); i++)
        {
            if (error)
            {
                return Value{numeric_limits<double>::quiet_NaN()};
            }
            if (holds_alternative<bool>(evaluateExpressionCalc(node->children[i], variables, error, inFunct)))
            {
                error = true;
                cout << "Runtime error: invalid operand type." << endl;
                return Value{numeric_limits<double>::quiet_NaN()};
            }
            Token opToken = node->token;
            double resultDouble = get<double>(result);
            if (opToken.text == "+")
            {
                resultDouble += get<double>(evaluateExpressionCalc(node->children[i], variables, error, inFunct));
            }
            else if (opToken.text == "-")
            {
                resultDouble -= get<double>(evaluateExpressionCalc(node->children[i], variables, error, inFunct));
            }
            else if (opToken.text == "*")
            {
                resultDouble *= get<double>(evaluateExpressionCalc(node->children[i], variables, error, inFunct));
            }
            else if (opToken.text == "/")
            {
                // Check for division by zero.
                Value denominator = evaluateExpressionCalc(node->children[i], variables, error, inFunct);
                if (get<double>(denominator) != 0)
                {
                    resultDouble /= get<double>(denominator);
                }
                else
                {
                    error = true;
                    cout << "Runtime error: division by zero." << endl;
                    return Value{numeric_limits<double>::quiet_NaN()};
                }
            }
            else if (opToken.text == "%")
            {
                resultDouble = fmod(resultDouble, get<double>(evaluateExpressionCalc(node->children[i], variables, error, inFunct)));
            }
            else
            {
                // If the operation is unrecognized, print an error message.
                printErrorCalc(opToken, error);
                return Value{numeric_limits<double>::quiet_NaN()};
            }
            result = Value{resultDouble};
        }
        return result;
    }
    else if (node->token.type == COMPARATOR)
    {
        // Iterate over the rest of the children to apply the operation.
        Value result = evaluateExpressionCalc(node->children[0], variables, error, inFunct);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            if (error)
            {
                return Value{numeric_limits<double>::quiet_NaN()};
            }
            Value childrenVal = evaluateExpressionCalc(node->children[i], variables, error, inFunct);
            Token opToken = node->token;
            if (opToken.text == "==" || opToken.text == "!=")
            {
                bool equality = true;
                // If different type or if same type but unequal, equality is false
                if (childrenVal.index() != result.index() ||
                    result != evaluateExpressionCalc(node->children[i], variables, error, inFunct))
                {
                    equality = false;
                }
                if (result.index() == 3 && childrenVal.index() == 3)
                {
                    equality = true;
                    if (get<shared_ptr<vector<Value>>>(result)->size() != get<shared_ptr<vector<Value>>>(childrenVal)->size())
                    {
                        equality = false;
                    }
                    else {
                        for (size_t j = 0; j < get<shared_ptr<vector<Value>>>(result)->size(); j++)
                        {
                            if (get<shared_ptr<vector<Value>>>(result)->at(j) != get<shared_ptr<vector<Value>>>(childrenVal)->at(j))
                            {
                                equality = false;
                            }
                        }
                    }
                }
                if (opToken.text == "==")
                {
                    result = Value{equality};
                }
                else if (opToken.text == "!=")
                {
                    result = Value{!equality};
                }
            }        
            else {
                // if left and right side of logical operator not the same type, return runtime error
                if ((holds_alternative<double>(childrenVal) && !holds_alternative<double>(result)) || (!holds_alternative<double>(childrenVal) && holds_alternative<double>(result)))
                {
                    error = true;
                    cout << "Runtime error: invalid operand type." << endl;
                    return Value{numeric_limits<double>::quiet_NaN()};
                }
                if (opToken.text == ">")
                {
                    result = Value{result > evaluateExpressionCalc(node->children[i], variables, error, inFunct)};
                }
                else if (opToken.text == "<")
                {
                    result = Value{result < evaluateExpressionCalc(node->children[i], variables, error, inFunct)};
                }
                else if (opToken.text == ">=")
                {
                    result = Value{result >= evaluateExpressionCalc(node->children[i], variables, error, inFunct)};
                }
                else if (opToken.text == "<=")
                {
                    // Check for division by zero.
                    result = Value{result <= evaluateExpressionCalc(node->children[i], variables, error, inFunct)};
                }
                else
                {
                    // If the operation is unrecognized, print an error message.
                    // cout << "unknown operator " << endl;
                    printErrorCalc(opToken, error);
                    return  Value{numeric_limits<double>::quiet_NaN()};
                }
            }
        }
        return result;
    }
    else if (node->token.type == LOGICAL)
    {
        // Iterate over the rest of the children to apply the operation.
        Value result = evaluateExpressionCalc(node->children[0], variables, error, inFunct);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            if (error)
            {
                return Value{numeric_limits<double>::quiet_NaN()};
            }
            Value childrenVal = evaluateExpressionCalc(node->children[i], variables, error, inFunct);
            if (childrenVal.index() != 1 || result.index() != 1)
            {
                if (!error)
                {
                    error = true;
                    cout << "Runtime error: invalid operand type." << endl;
                    return Value{numeric_limits<double>::quiet_NaN()};
                }
            }
            Token opToken = node->token;
            bool resultBool = get<bool>(result);
            if (opToken.text == "&")
            {
                resultBool = get<bool>(result) && get<bool>(evaluateExpressionCalc(node->children[i], variables, error, inFunct));
            }
            else if (opToken.text == "|")
            {
                resultBool = get<bool>(result) || get<bool>(evaluateExpressionCalc(node->children[i], variables, error, inFunct));
            }
            else
            {
                resultBool = get<bool>(result) != get<bool>(evaluateExpressionCalc(node->children[i], variables, error, inFunct));
            }
            result = Value{resultBool};
        }
        return result;
    }
    return  Value{numeric_limits<double>::quiet_NaN()};
}

Value lenCalc(Value array, bool& error) {
    if (array.index() != 3) {
        error = true;
        cout << "Runtime error: not an array." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    shared_ptr<vector<Value>> arrayPtr = get<shared_ptr<vector<Value>>>(array);
    // printValue(Value{double((*arrayPtr).size())});
    return Value{double((*arrayPtr).size())};
}
Value pushCalc(Value array, Value value, bool& error) {
    if (array.index() != 3) {
        error = true;
        cout << "Runtime error: not an array." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    shared_ptr<vector<Value>> arrayPtr = get<shared_ptr<vector<Value>>>(array);
    (*arrayPtr).push_back(value);
    return Value{nullptr};
}
Value popCalc(Value array, bool& error) {
    if (array.index() != 3) {
        error = true;
        cout << "Runtime error: not an array." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    if (int(get<double>(lenCalc(array, error))) == 0) {
        error = true;
        cout << "Runtime error: underflow." << endl;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    shared_ptr<vector<Value>> arrayPtr = get<shared_ptr<vector<Value>>>(array);
    Value back = (*arrayPtr).back();
    (*arrayPtr).pop_back();
    return back;
}

void printFunctCallCalc(const Node *node)
{
    const FunctCallNode* functCallNode = dynamic_cast<const FunctCallNode*>(node);
    if (functCallNode) {
        cout << functCallNode->functname.text << "(";
        for (size_t i = 0; i < functCallNode->arguments.size(); i++) {
            printInfixCalc(functCallNode->arguments[i]);
            if (i != functCallNode->arguments.size() - 1) {
                cout << ", ";
            }
        }
        cout << ")";
    }
}

// Prints the infix notation of a given AST.
void printInfixCalc(Node *node) 
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
    if (dynamic_cast<const FunctCallNode*>(node)) 
    {
        isFunctCall = true;
    }
    if (!isArrayAssignOrArrayLiteral && !isFunctCall && node && (node->token.type != FLOAT && node->token.type != IDENTIFIER && node->token.type != BOOLEAN && node->token.type != NULLVAL)) 
    {
        cout << "(";
    }
    printInfixHelperCalc(node);
    if (!isArrayAssignOrArrayLiteral && !isFunctCall && node && (node->token.type != FLOAT && node->token.type != IDENTIFIER && node->token.type != BOOLEAN && node->token.type != NULLVAL)) 
    {
        cout << ")";
    }
}

// Prints the infix notation of a given AST.
void printInfixHelperCalc(Node *node)
{
    //cout << " |" << node->token.text << "| ";
    if (!node)
    {
        return;
    }
    else if (const FunctCallNode *fcNode = dynamic_cast<const FunctCallNode*>(node))
    {
        printFunctCallCalc(fcNode);
    }
    else if (ArrayLiteralNode *aLNode = dynamic_cast<ArrayLiteralNode*>(node))
    {
        //cout << "length: " << aLNode->array.size() << endl;
        //cout << " HERE|" << node->token.text << "|HERE ";
        cout << "[";
        bool isArrayAssignOrArrayLiteral = false;
        for (size_t i = 0; i < aLNode->array.size(); i++)
        {
            Node *currNode = aLNode->array[i];
            if(dynamic_cast<ArrayLiteralNode*>(currNode))
            {
                isArrayAssignOrArrayLiteral = true;
            }   
            else if(dynamic_cast<ArrayAssignNode*>(currNode))
            {
                isArrayAssignOrArrayLiteral = true;
            }
            if (!isArrayAssignOrArrayLiteral && currNode && (currNode->token.type != FLOAT && currNode->token.type != IDENTIFIER && currNode->token.type != BOOLEAN && currNode->token.type != NULLVAL))
                cout << "(";
            printInfixHelperCalc(currNode);
            if (!isArrayAssignOrArrayLiteral && currNode && (currNode->token.type != FLOAT && currNode->token.type != IDENTIFIER && currNode->token.type != BOOLEAN && currNode->token.type != NULLVAL))
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
        printInfixHelperCalc(aANode->expression);
        cout << "[";
        printInfixCalc(aANode->arrayIndex);
        cout << "]";
    }
    else if (node->token.type == FLOAT)
    {
        double val = stod(node->token.text);
        if (val == static_cast<int>(val))
            cout << static_cast<int>(val);
        else
            cout << val;
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
            printInfixCalc(child);
        }
    }
}

// Prints a formatted error message for a given token
void printErrorCalc(const Token &token, bool &error)
{
    error = true;
    cout << "Unexpected token at line " << token.lineNumber
         << " column " << token.columnNumber << ": "
         << token.text << endl;
}

int main(int argc, const char **argv)
{
    string input;
    string text;
    unordered_map<string, Value> variables; // unordered_map<string, double> variables;

    while (getline(cin, input)) // Keep reading until EOF
    {
        vector<Token> tokens = readTokens(input);
        if (tokens.empty() || tokens.back().text == "error")
        {
            continue;
        }

        bool error = false;
        bool inFunct = false;
        int index = 0;
        Node *root;
        root = makeTreeCalc(tokens, index, error);
        // if (!checkVarCalc(root, error) || !checkParenCalc(tokens, error))
        if (!checkParenCalc(tokens, error))
        {
            // cout << "error from var or paren" << endl;
            deleteNodeCalc(root);
            continue;
        }
        if (root != nullptr && !error) // here
        {
            // cout << "past check var and paren" << endl;
            printInfixCalc(root);
            cout << endl;
        }

        Value result;
        if (checkIdenCalc(root, variables, error))
        {
            result = evaluateAllCalc(root, variables, error, inFunct);
        }
        if (!error)
        {
            printValueCalc(result);
            cout << endl;
        }
        deleteNodeCalc(root);
    }

    return 0;
}