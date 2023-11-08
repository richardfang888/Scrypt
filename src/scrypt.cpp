#include "lib/scrypt.hpp"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

using namespace std;

// variant<double, bool> AST::evaluateAST(unordered_map<string, variant<double, bool>> &variables)
// {
//     if (!&root)
//     {
//         return numeric_limits<double>::quiet_NaN();
//     }
//     return evaluate(root, variables);
// }

// Evaluates an if else block.
variant<double, bool> evaluateIfElse(IfElseNode *node, unordered_map<string, variant<double, bool>> &variables) 
{
    bool error = false;
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
            for (Node* statement : node->statementsTrue)
            {
                evaluateExpression(statement, variables, error);
            }
        }
        else
        {
            if (node->hasElse)
            {
                for (Node* statement : node->statementsFalse)
                {
                    evaluateExpression(statement, variables, error);
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
variant<double, bool> evaluateWhile(WhileNode *node, unordered_map<string, variant<double, bool>> &variables) 
{
    bool error = false;
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
                evaluateExpression(statement, variables, error);
            }
            condResult = evaluateExpression(node->condition, variables, error);
        }
    }
    // dummy return -> can make it void if needed
    return numeric_limits<double>::quiet_NaN();
}

// Evaluates a print statement.
variant<double, bool> evaluatePrint(PrintNode *node, unordered_map<string, variant<double, bool>> &variables) 
{
    bool error = false;
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
    if (!&node || error)
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
        printError(node->token, error);
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
                // cout << "unknown operator " << endl;
                printError(opToken, error);
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
                printError(opToken, error);
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

    return 0;
}