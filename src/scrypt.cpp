#include "lib/scrypt.hpp"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

using namespace std;

void evaluateAll(Node *node, unordered_map<string, variant<double, bool>> &variables, bool &error)
{
    if (IfElseNode *iENode = dynamic_cast<IfElseNode*>(node)) {
        // Node is a WhileNode
        // cout << "if exp" << endl;
        evaluateIfElse(iENode, variables, error);
    } else if (WhileNode *wNode = dynamic_cast<WhileNode*>(node)) {
        // Node is an IfElseNode
        // cout << "while exp" << endl;
        evaluateWhile(wNode, variables, error);
    } else if (PrintNode *pNode = dynamic_cast<PrintNode*>(node)) {
        // Node is a PrintNode
        // cout << "print exp" << endl;
        evaluatePrint(pNode, variables, error);
    } else {
        // Node is a normal Node
        // cout << "normal exp" << endl;
        evaluateExpression(node, variables, error);
    }
}

// Evaluates an if else block.
variant<double, bool> evaluateIfElse(IfElseNode *node, unordered_map<string, variant<double, bool>> &variables, bool &error) 
{
    if (!node || error)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    variant<double, bool> condResult = evaluateExpression(node->condition, variables, error);
    if (holds_alternative<double>(condResult))
    {
        // runtime error
        error = true;
        cout << "Runtime error: condition is not a bool." << endl;
        return numeric_limits<double>::quiet_NaN();
    }
    else if (holds_alternative<bool>(condResult)) 
    {
        if (get<bool>(condResult))
        {
            // cout << "if cond true" << endl;
            for (Node* statement : node->statementsTrue)
            {
                evaluateAll(statement, variables, error);
            }
        }
        else
        {
            // cout << "if false" << endl;
            if (node->hasElse)
            {
                // cout << "got to else" << endl;
                for (Node* statement : node->statementsFalse)
                {
                    evaluateAll(statement, variables, error);
                }
            }
            else
            {
                return numeric_limits<double>::quiet_NaN();
            }
        }
    }
    // dummy return -> can make it void if needed
    return numeric_limits<double>::quiet_NaN();
}

// Evaluates a while loop.
variant<double, bool> evaluateWhile(WhileNode *node, unordered_map<string, variant<double, bool>> &variables, bool &error) 
{
    if (!node || error)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    variant<double, bool> condResult = evaluateExpression(node->condition, variables, error);
    if (holds_alternative<double>(condResult))
    {
        // runtime error
        error = true;
        cout << "Runtime error: condition is not a bool." << endl;
        return numeric_limits<double>::quiet_NaN();
    }
    else if (holds_alternative<bool>(condResult)) 
    {
        while (get<bool>(condResult))
        {
            for (Node* statement : node->statements)
            {
                evaluateAll(statement, variables, error);
            }
            condResult = evaluateExpression(node->condition, variables, error);
            if (holds_alternative<double>(condResult))
            {
                // runtime error
                error = true;
                cout << "Runtime error: condition is not a bool." << endl;
                return numeric_limits<double>::quiet_NaN();
            }
        }
    }
    // dummy return -> can make it void if needed
    return numeric_limits<double>::quiet_NaN();
}

// Evaluates a print statement.
variant<double, bool> evaluatePrint(PrintNode *node, unordered_map<string, variant<double, bool>> &variables, bool &error) 
{
    if (!node || error)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    variant<double, bool> result = evaluateExpression(node->expression, variables, error);
    if (holds_alternative<double>(result))
    {
        cout << get<double>(result) << endl;
    }
    else if (holds_alternative<bool>(result))
    {
        cout << get<bool>(result) << endl;
    }
    // dummy return -> can make it void if needed
    return numeric_limits<double>::quiet_NaN();
}

// Evaluates an expression given the root of the expression's AST.
variant<double, bool> evaluateExpression(Node *node, unordered_map<string, variant<double, bool>> &variables, bool &error)
{
    if (!node || error)
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
            error = true;
            cout << "Runtime error: unknown identifier " + identifierText << endl;
            return numeric_limits<double>::quiet_NaN();
        }
    }
    // Node is an operator but has no children
    else if (node->children.size() == 0)
    {
        // cout << "no children" << endl;
        printErrorStatement(node->token, error);
        return numeric_limits<double>::quiet_NaN();
    }
    // Node is assignment operator
    else if (node->token.text == "=")
    {
        variant<double, bool> result = evaluateExpression(node->children[node->children.size() - 1], variables, error);
        for (int i = int(node->children.size() - 2); i >= 0; i--)
        {
            if (node->children[i]->token.type != IDENTIFIER)
            {
                // cout << "invalid identifier" << endl;
                printErrorStatement(node->token, error);

                return numeric_limits<double>::quiet_NaN();
            }
            variables[node->children[i]->token.text] = result;
        }
        return result;
    }
    // Node is a non-assignment operator
    else if (node->token.type == OPERATOR)
    {
        variant<double, bool> result = evaluateExpression(node->children[0], variables, error);
        if (holds_alternative<bool>(result))
        {
            // runtime error
            error = true;
            cout << "Runtime error: invalid operand type." << endl;
            return numeric_limits<double>::quiet_NaN();
        }
        // Iterate over the rest of the children to apply the operation.
        for (size_t i = 1; i < node->children.size(); i++)
        {
            if (holds_alternative<bool>(evaluateExpression(node->children[i], variables, error)))
            {
                error = true;
                cout << "Runtime error: invalid operand type." << endl;
                return numeric_limits<double>::quiet_NaN();
            }
            Token opToken = node->token;
            double resultDouble = get<double>(result);
            if (opToken.text == "+")
            {
                resultDouble += get<double>(evaluateExpression(node->children[i], variables, error));
            }
            else if (opToken.text == "-")
            {
                resultDouble -= get<double>(evaluateExpression(node->children[i], variables, error));
            }
            else if (opToken.text == "*")
            {
                resultDouble *= get<double>(evaluateExpression(node->children[i], variables, error));
            }
            else if (opToken.text == "/")
            {
                // Check for division by zero.
                variant<double, bool> denominator = evaluateExpression(node->children[i], variables, error);
                if (get<double>(denominator) != 0)
                {
                    resultDouble /= get<double>(denominator);
                }
                else
                {
                    error = true;
                    cout << "Runtime error: division by zero." << endl;
                    return numeric_limits<double>::quiet_NaN();
                }
            }
            else if (opToken.text == "%") {
                resultDouble = fmod(resultDouble, get<double>(evaluateExpression(node->children[i], variables, error)));
            }
            else
            {
                // If the operation is unrecognized, print an error message.
                // cout << "unknown logical " << endl;
                printErrorStatement(opToken, error);
                return numeric_limits<double>::quiet_NaN();
            }
            result = resultDouble;
        }
        return result;
    }
    else if (node->token.type == COMPARATOR) 
    {
        // Iterate over the rest of the children to apply the operation.
        variant<double, bool> result = evaluateExpression(node->children[0], variables, error);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            variant<double, bool> childrenVal = evaluateExpression(node->children[i], variables, error);
            if ((holds_alternative<double>(childrenVal) && !holds_alternative<double>(result)) || (!holds_alternative<double>(childrenVal) && holds_alternative<double>(result)))
            {
                error = true;
                cout << "Runtime error: invalid operand type." << endl;
                return numeric_limits<double>::quiet_NaN();
            }
            Token opToken = node->token;
            if (opToken.text == ">")
            {
                result = result > evaluateExpression(node->children[i], variables, error);
            }
            else if (opToken.text == "<")
            {
                result = result < evaluateExpression(node->children[i], variables, error);
            }
            else if (opToken.text == ">=")
            {
                result =  result >= evaluateExpression(node->children[i], variables, error);
            }
            else if (opToken.text == "<=")
            {
                // Check for division by zero.
                result = result <= evaluateExpression(node->children[i], variables, error);
            }
            else if (opToken.text == "==")
            {
                result = result == evaluateExpression(node->children[i], variables, error);
            }
            else if (opToken.text == "!=")
            {
                result = result != evaluateExpression(node->children[i], variables, error);
            }
            else
            {
                // If the operation is unrecognized, print an error message.
                // cout << "unknown operator " << endl;
                printErrorStatement(opToken, error);
                return numeric_limits<double>::quiet_NaN();
            }
        }
        return result;
    }
    else if (node->token.type == LOGICAL) 
    {
        // Iterate over the rest of the children to apply the operation.
        variant<double, bool> result = evaluateExpression(node->children[0], variables, error);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            variant<double, bool> childrenVal = evaluateExpression(node->children[i], variables, error);
            if (holds_alternative<double>(childrenVal) || holds_alternative<double>(result))
            {
                error = true;
                cout << "Runtime error: invalid operand type." << endl;
                return numeric_limits<double>::quiet_NaN();
            }
            Token opToken = node->token;
            bool resultBool = get<bool>(result);
            if (opToken.text == "&")
            {
                resultBool = get<bool>(result) && get<bool>(evaluateExpression(node->children[i], variables, error));
            }
            else if (opToken.text == "|")
            {
                resultBool = get<bool>(result) || get<bool>(evaluateExpression(node->children[i], variables, error));
            }
            else
            {
                resultBool = get<bool>(result) != get<bool>(evaluateExpression(node->children[i], variables, error));
            }
            result = resultBool;
        }
        return result;
    }
    return numeric_limits<double>::quiet_NaN();
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
    //text = "x = 42 \n steps = 0 \n while x > 1 { \n steps = steps + 1 \n if x % 2 == 0 { \n x = x / 2 \n } \n else { \n x = 3 * x + 1 \n } \n } \n ";
    //text = "steps = 0 \n while steps < 3 { \n steps = steps + 1 \n print steps \n 5 / 9 \n } \n 4 - 7 ";
    //text = "x = 42 \n steps = 0 \n if steps < 3 { \n steps = steps + 1 \n } \n else { \n x = 3 * x + 1 \n }";
    //text = "x = 42 \n steps = 0 \n if steps < 3 { \n steps = steps + 1 \n } \n ";
    // text = "val  = 105 \n fizz = val % 3 == 0 \n buzz = false \n if val % 5 == 0 { \n buzz = true \n } \n  if fizz & buzz { \n if buzz { \n print 333555 \n } \n else { \n print 333 \n } \n } \n else if buzz { \n print 555 \n } \n else { \n print val \n }";
    //text = "val  = 105 \n fizz = val % 3 == 0 \n buzz = false \n if val % 5 == 0 { \n buzz = true \n } \n  if fizz & buzz { \n if buzz { \n print 333555 \n } \n else { \n print 333 \n } \n } \n ";
    //text = "val  = 105 \n fizz = val % 3 == 0 \n buzz = false \n if val % 5 == 0 { \n buzz = true \n } \n 4 + 5";
    //text = "some_var = 17 \n print some_var \n \n while some_var > 1.00 { \n some_var = some_var / 2 \n print some_var} \n";
    //text = "print 22/7 \n";
    //text = "x = 42 \n steps = 0 \n while x > 1 { \n steps = steps + 1 \n if x % 2 == 0 { \n x = x / 2 \n } \n else { \n x = 3 * x + 1 \n } \n } \n print steps \n";
    //text = "print a = 49 \n print b = 21 \n while a != b {\n if a > b {\n a = a - b \n } \n else if b > a {\n b = b - a \n } \n } \n print a \n";
    // text = "a = 12 \n b = 14 \n c = true \n \n while c { \n print a \n if a < b { \n a = a + 7 \n } \n else if a > b { \n b = b + 5 \n } \n else { \n c = 0\n } \n }";

    // lex
    tokens = readTokens(text);

    // set up variables for muti expression parsing
    
    int index = 0;
    vector<Node*> trees;
    std::unordered_map<string, variant<double, bool>> variables;
    if (tokens.empty() || tokens.back().text == "error") //
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

    //print and evaluate trees
    for (size_t i = 0; i < trees.size(); i++)
    {
        bool error = false;
        evaluateAll(trees[i], variables, error);
    }
    for (size_t i = 0; i < trees.size(); i++)
    {  
        deleteNodeAll(trees[i]);
    }
    return 0;
}