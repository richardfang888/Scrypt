#include "lib/scrypt.hpp"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

using namespace std;

Value evaluateAll(Node *node, unordered_map<string, Value> &variables, bool &error)
{
    if (IfElseNode *iENode = dynamic_cast<IfElseNode *>(node))
    {
        // Node is an IfElseNode
        evaluateIfElse(iENode, variables, error);
    }
    else if (WhileNode *wNode = dynamic_cast<WhileNode *>(node))
    {
        // Node is a WhileNode
        evaluateWhile(wNode, variables, error);
    }
    else if (PrintNode *pNode = dynamic_cast<PrintNode *>(node))
    {
        // Node is a PrintNode
        evaluatePrint(pNode, variables, error);
    }
    else if (ReturnNode *rNode = dynamic_cast<ReturnNode *>(node))
    {
        // Node is a ReturnNode
        return evaluateReturn(rNode, variables, error);
    }
    else if (FunctDefNode *fdNode = dynamic_cast<FunctDefNode *>(node))
    {
        // Node is a FunctDefNode
        evaluateFunctDef(fdNode, variables, error);
    }
    else if (FunctCallNode *fcNode = dynamic_cast<FunctCallNode *>(node))
    {
        // Node is a FunctCallNode
        evaluateFunctCall(fcNode, variables, error);
    }
    else
    {
        // Node is a normal Node
        evaluateExpression(node, variables, error);
    }
    return numeric_limits<double>::quiet_NaN();
}

// Evaluates an if else block.
Value evaluateIfElse(IfElseNode *node, unordered_map<string, Value> &variables, bool &error)
{
    if (!node || error)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    Value condResult = evaluateExpression(node->condition, variables, error);
    if (holds_alternative<double>(condResult))
    {
        // runtime error
        error = true;
        cout << "Runtime error: condition is not a bool." << endl;
        exit(3);
        return numeric_limits<double>::quiet_NaN();
    }
    else if (holds_alternative<bool>(condResult))
    {
        if (get<bool>(condResult))
        {
            // change to evaluateStatements
            for (Node *statement : node->statementsTrue)
            {
                evaluateAll(statement, variables, error);
            }
        }
        else
        {
            if (node->hasElse)
            {
                // change to evaluateStatements
                for (Node *statement : node->statementsFalse)
                {
                    evaluateAll(statement, variables, error);
                }
            }
        }
    }
    // dummy return
    return numeric_limits<double>::quiet_NaN();
}

// Evaluates a while loop.
Value evaluateWhile(WhileNode *node, unordered_map<string, Value> &variables, bool &error)
{
    if (!node || error)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    Value condResult = evaluateExpression(node->condition, variables, error);
    if (holds_alternative<double>(condResult))
    {
        // runtime error
        error = true;
        cout << "Runtime error: condition is not a bool." << endl;
        exit(3);
        return numeric_limits<double>::quiet_NaN();
    }
    else if (holds_alternative<bool>(condResult))
    {
        while (get<bool>(condResult))
        {
            // change to evaluateStatements (set to var, check if return is nan again, return if not)
            for (Node *statement : node->statements)
            {
                evaluateAll(statement, variables, error);
            }
            condResult = evaluateExpression(node->condition, variables, error);
            if (holds_alternative<double>(condResult))
            {
                // runtime error
                error = true;
                cout << "Runtime error: condition is not a bool." << endl;
                exit(3);
                return numeric_limits<double>::quiet_NaN();
            }
        }
    }
    // dummy return
    return numeric_limits<double>::quiet_NaN();
}

// Evaluates a print statement.
Value evaluatePrint(PrintNode *node, unordered_map<string, Value> &variables, bool &error)
{
    if (!node || error)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    Value result = evaluateExpression(node->expression, variables, error);
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

// Evaluates a return statement.
Value evaluateReturn(ReturnNode *node, unordered_map<string, Value> &variables, bool &error)
{
    if (!node || error)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    Value result = evaluateExpression(node->expression, variables, error);

    // set the value of the function call to return
    return result;
}

// Evaluates a function definition.
Value evaluateFunctDef(FunctDefNode *node, unordered_map<string, Value> &variables, bool &error)
{
    if (!node || error)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    // add function to defined functions
    Function func;
    func.function = node;
    variables[node->functname.text] = Value{make_shared<Function>(func)};
    cout << "Function " << node->functname.text << " defined." << endl;

    // dummy return
    return numeric_limits<double>::quiet_NaN();
}

// Evaluates a function call.
Value evaluateFunctCall(FunctCallNode *node, unordered_map<string, Value> &variables, bool &error)
{
    if (!node || error)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    
    // check if function exists
    cout << "Function " << node->functname.text << " called." << endl;
    auto iter = variables.find(node->functname.text);
    if (iter == variables.end()) 
    {
        error = true;
        // function undefined
        cout << "Runtime error: function" << node->functname.text << " not defined." << endl;
        exit(3);
        return numeric_limits<double>::quiet_NaN();
    }

    Value functionValue = iter->second;
    if (!holds_alternative<shared_ptr<Function>>(functionValue))
    {
        error = true;
        // function undefined
        cout << "Runtime error: function " << node->token.text << " not defined." << endl;
        exit(3);
        return numeric_limits<double>::quiet_NaN();
    }
    shared_ptr<Function> function = get<shared_ptr<Function>>(functionValue);
    
    // evaluate arguments
    vector<Value> evaluatedArguments;
    for (Node *argument : node->arguments)
    {
        Value evaluatedArgument = evaluateAll(argument, variables, error);
        evaluatedArguments.push_back(evaluatedArgument);
    }
    
    // call function
    if (!function->function) {
        error = true;
        return numeric_limits<double>::quiet_NaN();
    }
    // check if arguments match
    if (evaluatedArguments.size() != function->function->params.size()) {
        error = true;
        return numeric_limits<double>::quiet_NaN();
    }
    // create new scope for function call
    for (size_t i = 0; i < function->function->params.size(); i++)
    {
        function->functVariables[function->function->params[i]] = evaluatedArguments[i];
    }
    // Evaluate the statements within the function scope
    for (Node *statement : function->function->statements)
    {
        Value result = evaluateAll(statement, function->functVariables, error);
        if (error) {
            // Handle errors that occurred during evaluation
            return numeric_limits<double>::quiet_NaN();
        }
        if (result.index() != 0 || get<double>(result) != numeric_limits<double>::quiet_NaN()) {
            return result;
        }
    }

    return numeric_limits<double>::quiet_NaN();
}


// Evaluates an expression given the root of the expression's AST.
Value evaluateExpression(Node *node, unordered_map<string, Value> &variables, bool &error)
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
    if (FunctCallNode *fcNode = dynamic_cast<FunctCallNode *>(node))
    {
        // Node is a FunctCallNode
        return evaluateFunctCall(fcNode, variables, error);
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
            exit(3);
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
        Value result = evaluateExpression(node->children[node->children.size() - 1], variables, error);
        for (int i = int(node->children.size() - 2); i >= 0; i--)
        {
            if (node->children[i]->token.type != IDENTIFIER)
            {
                error = true;
                cout << "Runtime error: invalid asignee." << endl;
                exit(3);

                return numeric_limits<double>::quiet_NaN();
            }
            variables[node->children[i]->token.text] = result;
        }
        return result;
    }
    // Node is a non-assignment operator
    else if (node->token.type == OPERATOR)
    {
        Value result = evaluateExpression(node->children[0], variables, error);
        if (holds_alternative<bool>(result))
        {
            // runtime error
            error = true;
            cout << "Runtime error: invalid operand type." << endl;
            exit(3);
            return numeric_limits<double>::quiet_NaN();
        }
        // Iterate over the rest of the children to apply the operation.
        for (size_t i = 1; i < node->children.size(); i++)
        {
            if (holds_alternative<bool>(evaluateExpression(node->children[i], variables, error)))
            {
                error = true;
                cout << "Runtime error: invalid operand type." << endl;
                exit(3);
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
                Value denominator = evaluateExpression(node->children[i], variables, error);
                if (get<double>(denominator) != 0)
                {
                    resultDouble /= get<double>(denominator);
                }
                else
                {
                    error = true;
                    cout << "Runtime error: division by zero." << endl;
                    exit(3);
                    return numeric_limits<double>::quiet_NaN();
                }
            }
            else if (opToken.text == "%")
            {
                resultDouble = fmod(resultDouble, get<double>(evaluateExpression(node->children[i], variables, error)));
            }
            else
            {
                // If the operation is unrecognized, print an error message.
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
        Value result = evaluateExpression(node->children[0], variables, error);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            Value childrenVal = evaluateExpression(node->children[i], variables, error);
            Token opToken = node->token;
            if (opToken.text == "==" || opToken.text == "!=")
            {
                bool equality = true;
                // If different type or if same type but unequal, equality is false
                if ((holds_alternative<double>(childrenVal) && !holds_alternative<double>(result)) || 
                    (!holds_alternative<double>(childrenVal) && holds_alternative<double>(result)) ||
                    result != evaluateExpression(node->children[i], variables, error))
                {
                    equality = false;
                }
                if (opToken.text == "==")
                {
                    result = equality;
                }
                else if (opToken.text == "!=")
                {
                    result = !equality;
                }
            }        
            else {
                // if left and right side of logical operator not the same type, return runtime error
                if ((holds_alternative<double>(childrenVal) && !holds_alternative<double>(result)) || (!holds_alternative<double>(childrenVal) && holds_alternative<double>(result)))
                {
                    error = true;
                    cout << "Runtime error: invalid operand type." << endl;
                    exit(3);
                    return numeric_limits<double>::quiet_NaN();
                }
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
                else
                {
                    // If the operation is unrecognized, print an error message.
                    // cout << "unknown operator " << endl;
                    printErrorStatement(opToken, error);
                    return numeric_limits<double>::quiet_NaN();
                }
            }
        }
        return result;
    }
    else if (node->token.type == LOGICAL)
    {
        // Iterate over the rest of the children to apply the operation.
        Value result = evaluateExpression(node->children[0], variables, error);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            Value childrenVal = evaluateExpression(node->children[i], variables, error);
            if (holds_alternative<double>(childrenVal) || holds_alternative<double>(result))
            {
                error = true;
                cout << "Runtime error: invalid operand type." << endl;
                exit(3);
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

    // while (getline(cin, input))
    // {
    //     text += input;
    //     if (!cin.eof())
    //     {
    //         text += '\n';
    //     }
    // }

    // Old statements tests

    // Functions tests
    text = "def say_no() {\nprint false;\n}\ndef say_even(x) {\nprint x % 2 == 0;\n}\ndef say_equal(x, y, z) {\nprint x == y & y == z;\n}\nsay_even(14);\nsay_even(1 + 2);\nsay_no();\nsay_equal(4.0, 4, 4.00000);\nsay_equal(3, 3.00, 3.00001);";

    // lex
    tokens = readTokens(text);

    // set up variables for muti expression parsing

    int index = 0;
    vector<Node *> trees;
    unordered_map<string, Value> variables;
    vector<FunctDefNode *> functions;
    if (tokens.empty() || tokens.back().text == "error") //
    {
        exit(1);
    }

    // parse the tokens and put into trees
    while (tokens[index].type != END)
    {
        Node *root;
        root = makeTree(tokens, index);
        trees.push_back(root);
        index++;
    }

    // print and evaluate trees
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