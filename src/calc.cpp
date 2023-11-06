#include "lib/calc.hpp"
#include <iostream>
#include <limits>
#include <cmath>

AST::AST(const vector<Token> &tokens)
{
    error = false;
    int index = 0;
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

// Recursively creates an AST from a list of tokens
Node *AST::makeTree(const vector<Token> &tokens, int &index)
{
    return parseAssignment(tokens, index);
}

// Function to parse assignment expressions
Node *AST::parseAssignment(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseLogicAnd(tokens, index);
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
Node *AST::parseLogicAnd(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseLogicXor(tokens, index);
    while (match(tokens, index, "&") || match(tokens, index, "|") || match(tokens, index, "^"))
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
    Node *left = parseLogicOr(tokens, index);
    while (match(tokens, index, "&") || match(tokens, index, "|") || match(tokens, index, "^"))
    {
        Token opToken = tokens[index++];
        Node *right = parseLogicOr(tokens, index);
        Node *opNode = makeNode(opToken);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
    return left;
}

Node *AST::parseLogicOr(const vector<Token> &tokens, int &index)
{
    if (error)
    {
        return nullptr;
    }
    Node *left = parseEquality(tokens, index);
    while (match(tokens, index, "&") || match(tokens, index, "|") || match(tokens, index, "^"))
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
                cout << "parenthesis" << endl;
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
        cout << "primary" << endl;
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

variant<double, bool> AST::evaluateAST(unordered_map<string, variant<double, bool>> &variables)
{
    if (!root)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    return evaluate(root, variables);
}

// Evaluates the given AST node and returns the result of the original expression.
variant<double, bool> AST::evaluate(Node *node, unordered_map<string, variant<double, bool>> &variables)
{
    if (!node)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    // If the node holds a FLOAT token, simply return its value.
    if (node->token.type == FLOAT)
    {
        return stod(node->token.text);
    }
    // If the node holds a BOOLEAN token, simply return its value.
    if (node->token.type == BOOLEAN)
    {
        if (node->token.text == "true")
        {
            return true;
        }
        else
        {
            return false;
        }
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
            cout << "Runtime error: unknown identifier " + identifierText << endl;
            return numeric_limits<double>::quiet_NaN();
        }
    }
    // Node is an operator but has no children
    else if (node->children.size() == 0)
    {
        printError(node->token, error);
        return numeric_limits<double>::quiet_NaN();
    }
    // Node is assignment operator
    else if (node->token.text == "=")
    {
        variant<double, bool> result = evaluate(node->children[node->children.size() - 1], variables);
        for (int i = int(node->children.size() - 2); i >= 0; i--)
        {
            if (node->children[i]->token.type != IDENTIFIER)
            {
                // invalid assignment error
                printError(node->token, error);
                return numeric_limits<double>::quiet_NaN();
            }
            variables[node->children[i]->token.text] = result;
        }
        return result;
    }
    // Node is a non-assignment operator
    else if (node->token.type == OPERATOR)
    {
        variant<double, bool> result = evaluate(node->children[0], variables);
        if (holds_alternative<bool>(result))
        {
           // runtime error
        }
        // Iterate over the rest of the children to apply the operation.
        for (size_t i = 1; i < node->children.size(); i++)
        {
            if (holds_alternative<bool>(result))
            {
                // runtime error
                break;
            }
            Token opToken = node->token;
            double resultDouble = get<double>(result);
            if (opToken.text == "+")
            {
                resultDouble += get<double>(evaluate(node->children[i], variables));
            }
            else if (opToken.text == "-")
            {
                resultDouble -= get<double>(evaluate(node->children[i], variables));
            }
            else if (opToken.text == "*")
            {
                resultDouble *= get<double>(evaluate(node->children[i], variables));
            }
            else if (opToken.text == "/")
            {
                // Check for division by zero.
                variant<double, bool> denominator = evaluate(node->children[i], variables);
                if (get<double>(denominator) != 0)
                {
                    resultDouble /= get<double>(denominator);
                }
                else
                {
                    cout << "Runtime error: division by zero." << endl;
                    return numeric_limits<double>::quiet_NaN();
                }
            }
            else if (opToken.text == "%") {
                resultDouble = fmod(resultDouble, get<double>(evaluate(node->children[i], variables)));
            }
            else
            {
                // If the operation is unrecognized, print an error message.
                cout << "unknown operator " << endl;
                printError(opToken, error);
                return numeric_limits<double>::quiet_NaN();
            }
        }
        return result;
    }
    else if (node->token.type == COMPARATOR) {
        // Iterate over the rest of the children to apply the operation.
        variant<double, bool> result = evaluate(node->children[0], variables);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            variant<double, bool> childrenVal = evaluate(node->children[i], variables);
            if ((holds_alternative<double>(childrenVal) && !holds_alternative<double>(result)) || (!holds_alternative<double>(childrenVal) && holds_alternative<double>(result)))
            {
                // runtime error
            }
            Token opToken = node->token;
            if (opToken.text == ">")
            {
                result = result > evaluate(node->children[i], variables);
            }
            else if (opToken.text == "<")
            {
                result = result < evaluate(node->children[i], variables);
            }
            else if (opToken.text == ">=")
            {
                result =  result >= evaluate(node->children[i], variables);
            }
            else if (opToken.text == "<=")
            {
                // Check for division by zero.
                result = result <= evaluate(node->children[i], variables);
            }
            else
            {
                // If the operation is unrecognized, print an error message.
                cout << "unknown operator " << endl;
                printError(opToken, error);
                return numeric_limits<double>::quiet_NaN();
            }
        }
        return result;
    }
    else if (node->token.type == LOGICAL) {
        // Iterate over the rest of the children to apply the operation.
        variant<double, bool> result = evaluate(node->children[0], variables);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            variant<double, bool> childrenVal = evaluate(node->children[i], variables);
            if (holds_alternative<double>(childrenVal) || holds_alternative<double>(result))
            {
                // runtime error
            }
            Token opToken = node->token;
            bool resultBool = get<bool>(result);
            if (opToken.text == "&")
            {
                resultBool = get<bool>(result) && get<bool>(evaluate(node->children[i], variables));
            }
            else if (opToken.text == "|")
            {
                resultBool = get<bool>(result) || get<bool>(evaluate(node->children[i], variables));
            }
            else
            {
                resultBool = get<bool>(result) != get<bool>(evaluate(node->children[i], variables));
            }
            result = resultBool;
        }
        return result;
    }
    return numeric_limits<double>::quiet_NaN();
}

Node *AST::getRoot() const
{
    return root;
}

void AST::printInfix() const
{
    if (root && (root->token.type != FLOAT && root->token.type != IDENTIFIER && root->token.type != BOOLEAN))
        cout << "(";

    printInfix(root);
    if (root && (root->token.type != FLOAT && root->token.type != IDENTIFIER && root->token.type != BOOLEAN))
        cout << ")";

    cout << endl;
}

// Prints the infix notation of a given AST.
void AST::printInfix(const Node *node) const
{
    if (!node)
    {
        return;
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
            if (child->token.type != FLOAT && child->token.type != IDENTIFIER && child->token.type != BOOLEAN)
            {
                cout << "(";
            }
            printInfix(child);
            if (child->token.type != FLOAT && child->token.type != IDENTIFIER && child->token.type != BOOLEAN)
            {
                cout << ")";
            }
        }
    }
}

// Prints a formatted error message for a given token
void printError(const Token &token, bool &error)
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
    unordered_map<string, variant<double, bool>> variables; // unordered_map<string, double> variables;

    while (getline(cin, input)) // Keep reading until EOF
    {
        vector<Token> tokens = readTokens(input);

        AST ast(tokens);
        if (!ast.checkVar(ast.getRoot()))
        {
            continue;
        }
        if (ast.getRoot() != nullptr && !ast.error)
        {
            ast.printInfix();
        }
        variant<double, bool> result = numeric_limits<double>::quiet_NaN();
        if (ast.checkIden(ast.getRoot(), variables))
        {
            result = ast.evaluateAST(variables);
        }
        if (holds_alternative<double>(result)) {
            if (!isnan(get<double>(result)))
            {
                cout << get<double>(result) << endl;
            }
        }
        else {
            if (get<bool>(result)) {
                cout << "true" << endl;
            }
            else {
                cout << "false" << endl;
            }
        }
    }

    return 0;
}