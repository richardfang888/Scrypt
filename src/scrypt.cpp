#include "lib/scrypt.hpp"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

using namespace std;

double AST::evaluateAST(unordered_map<string, double> &variables)
{
    if (!root)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    return evaluate(root, variables);
}

// Evaluates the given AST node and returns the result of the original expression.
double AST::evaluate(Node *node, unordered_map<string, double> &variables)
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
        double result = evaluate(node->children[node->children.size() - 1], variables);
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
    else
    {
        // Iterate over the rest of the children to apply the operation.
        double result = evaluate(node->children[0], variables);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            Token opToken = node->token;
            if (opToken.text == "+")
            {
                result += evaluate(node->children[i], variables);
            }
            else if (opToken.text == "-")
            {
                result -= evaluate(node->children[i], variables);
            }
            else if (opToken.text == "*")
            {
                result *= evaluate(node->children[i], variables);
            }
            else if (opToken.text == "/")
            {
                // Check for division by zero.
                double denominator = evaluate(node->children[i], variables);
                if (denominator != 0)
                {
                    result /= denominator;
                }
                else
                {
                    cout << "Runtime error: division by zero." << endl;
                    return numeric_limits<double>::quiet_NaN();
                }
            }
            else if (opToken.text == "%") {
                result = fmod(result, evaluate(node->children[i], variables));
            }
            else
            {
                // If the operation is unrecognized, print an error message.
                printError(opToken, error);
                return numeric_limits<double>::quiet_NaN();
            }
        }
        return result;
    }
    return numeric_limits<double>::quiet_NaN();
}

int main(int argc, const char **argv)
{

    return 0;
}