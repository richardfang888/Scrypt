#include "lib/scrypt.hpp"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

using namespace std;

Value returnVal = Value{numeric_limits<double>::quiet_NaN()};

Value evaluateAll(Node *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    if (IfElseNode *iENode = dynamic_cast<IfElseNode *>(node))
    {
        // Node is an IfElseNode
        return evaluateIfElse(iENode, variables, error, inFunct);
    }
    else if (WhileNode *wNode = dynamic_cast<WhileNode *>(node))
    {
        // Node is a WhileNode
        return evaluateWhile(wNode, variables, error, inFunct);
    }
    else if (PrintNode *pNode = dynamic_cast<PrintNode *>(node))
    {
        // Node is a PrintNode
        return evaluatePrint(pNode, variables, error, inFunct);
    }
    else if (ReturnNode *rNode = dynamic_cast<ReturnNode *>(node))
    {
        // Node is a ReturnNode
        return evaluateReturn(rNode, variables, error, inFunct);
    }
    else if (FunctDefNode *fdNode = dynamic_cast<FunctDefNode *>(node))
    {
        // Node is a FunctDefNode
        return evaluateFunctDef(fdNode, variables, error, inFunct);
    }
    else if (FunctCallNode *fcNode = dynamic_cast<FunctCallNode *>(node))
    {
        // Node is a FunctCallNode
        return evaluateFunctCall(fcNode, variables, error, inFunct);
    }
    else if (ArrayLiteralNode *alNode = dynamic_cast<ArrayLiteralNode *>(node))
    {
        // Node is an ArrayLiteralNode
        return evaluateArrayLiteral(alNode, variables, error, inFunct);
    }
    else if (ArrayAssignNode *alNode = dynamic_cast<ArrayAssignNode *>(node))
    {
        // Node is an ArrayAssignNode
        return evaluateArrayAssign(alNode, variables, error, inFunct, false);
    }
    else
    {
        // Node is a normal Node
        return evaluateExpression(node, variables, error, inFunct);
    }
    return Value{numeric_limits<double>::quiet_NaN()};
}

// Evaluates an if else block.
Value evaluateIfElse(IfElseNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    // cout << "eval ifElse" << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    Value condResult = evaluateExpression(node->condition, variables, error, inFunct);
    if (condResult.index() != 1)
    {
        // runtime error
        error = true;
        cout << "Runtime error: condition is not a bool." << endl;
        exit(3);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    else if (holds_alternative<bool>(condResult))
    {
        if (get<bool>(condResult))
        {
            // change to evaluateStatements
            for (Node *statement : node->statementsTrue)
            {
                evaluateAll(statement, variables, error, inFunct);
            }
        }
        else
        {
            if (node->hasElse)
            {
                // change to evaluateStatements
                for (Node *statement : node->statementsFalse)
                {
                    evaluateAll(statement, variables, error, inFunct);
                }
            }
        }
    }
    // dummy return
    return Value{numeric_limits<double>::quiet_NaN()};
}

// Evaluates a while loop.
Value evaluateWhile(WhileNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    // cout << "eval while" << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    Value condResult = evaluateExpression(node->condition, variables, error, inFunct);
    // printValue(condResult);
    if (condResult.index() != 1)
    {
        // runtime error
        error = true;
        cout << "Runtime error: condition is not a bool." << endl;
        exit(3);
        return  Value{numeric_limits<double>::quiet_NaN()};
    }
    else if (holds_alternative<bool>(condResult))
    {
        while (get<bool>(condResult))
        {
            // change to evaluateStatements (set to var, check if return is nan again, return if not)
            for (Node *statement : node->statements)
            {
                evaluateAll(statement, variables, error, inFunct);
            }
            condResult = evaluateExpression(node->condition, variables, error, inFunct);
            if (condResult.index() != 1)
            {
                // runtime error
                error = true;
                cout << "Runtime error: condition is not a bool." << endl;
                exit(3);
                return Value{numeric_limits<double>::quiet_NaN()};
            }
        }
    }
    // dummy return
    return Value{numeric_limits<double>::quiet_NaN()};
}

// Evaluates a print statement.
Value evaluatePrint(PrintNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    // cout << "eval print" << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    Value result = evaluateExpression(node->expression, variables, error, inFunct);
    // cout << "print value: " << get<double>(result) << endl;
    printValue(result);
    cout << endl;
    // dummy return -> can make it void if needed
    return Value{numeric_limits<double>::quiet_NaN()};
}

// helper function for evaluate print
void printValue(Value value) 
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
                printValue(vec[i]);
            }
            else
            {
                printValue(vec[i]);
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
Value evaluateReturn(ReturnNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
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
        exit(3);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    if (!node->expression)
    {
        return Value{nullptr};
    }
    Value result = evaluateExpression(node->expression, variables, error, inFunct);
    returnVal = result;

    // set the value of the function call to return
    return result;
}

// Evaluates a function definition.
Value evaluateFunctDef(FunctDefNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
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
Value evaluateFunctCall(FunctCallNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    // cout << "Function " << node->functname.text << " called." << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }

    // debugging scope
    // cout << "variables scope: " << endl;
    // for (auto const& x : variables) {
    //     cout << x.first << ": ";
    //     printValue(x.second);
    //     cout << endl;
    // }

    if (node->functname.text == "len" || node->functname.text == "push" || node->functname.text == "pop")
    {
        return evaluateUtilityFunct(node, variables, error, inFunct);
    }

    // check if function exists
    auto iter = variables.find(node->functname.text);
    if (iter == variables.end()) 
    {
        // cout << "can't find: " << node->functname.text << endl;
        error = true;
        cout << "Runtime error: not a function." << endl;
        exit(3);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    Value functionValue = iter->second;
    // check if function is a function type
    if (!holds_alternative<shared_ptr<Function>>(functionValue))
    {
        error = true;
        cout << "Runtime error: not a function." << endl;
        exit(3);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    shared_ptr<Function> function = get<shared_ptr<Function>>(functionValue);
    
    // check if number of arguments matches
    if (node->arguments.size() != function->function->params.size()) 
    {
        error = true;
        cout << "Runtime error: incorrect argument count." << endl;
        exit(3);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    // evaluate arguments
    vector<Value> evaluatedArguments;
    for (Node *argument : node->arguments)
    {
        Value result = evaluateAll(argument, variables, error, inFunct);
        // cout << "argument: ";
        // printValue(result);
        // cout << endl;
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
    
    // debugging scope
    // cout << "function scope: " << endl;
    // for (auto const& x : function->functVariables) {
    //     cout << x.first << ": ";
    //     printValue(x.second);
    //     cout << endl;
    // }

    inFunct = true;
    // Evaluate the statements within the function scope
    for (Node *statement : function->function->statements)
    {
        Value result = evaluateAll(statement, newVariables, error, inFunct);
        if (error) {
            // Handle errors that occurred during evaluation
            return Value{numeric_limits<double>::quiet_NaN()};
        }
        if (returnVal.index() != 0 || !isnan(get<double>(returnVal))) {
            Value functReturn = returnVal;
            // cout << "actual return ";
            // printValue(functReturn);
            // cout << endl;
            returnVal = Value{numeric_limits<double>::quiet_NaN()};
            return functReturn;
        }
    }
    inFunct = false;
    return Value{nullptr};
}

Value evaluateUtilityFunct(FunctCallNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
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
            Value array = evaluateAll(node->arguments[0], variables, error, inFunct);
            return len(array);
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
            Value array = evaluateAll(node->arguments[0], variables, error, inFunct);
            Value value = evaluateAll(node->arguments[1], variables, error, inFunct);
            return push(array, value);
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
            Value array = evaluateAll(node->arguments[0], variables, error, inFunct);
            return pop(array);
        }
    }

    if (argCountError)
    {
        error = true;
        cout << "Runtime error: incorrect argument count." << endl;
        exit(3);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    return Value{nullptr};
}


// Evaluates an array literal
Value evaluateArrayLiteral(ArrayLiteralNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct) 
{
    // cout << "eval array literal" << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    vector<Value> evaluatedArray;
    for (Node *element : node->array)
    {
        evaluatedArray.push_back(evaluateAll(element, variables, error, inFunct));
    }
    return Value{make_shared<vector<Value>>(evaluatedArray)};
}

Value evaluateArrayAssign(ArrayAssignNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct, bool setValue)
{
    // cout << "eval array assign" << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    Value arrayIndexValue = evaluateAll(node->arrayIndex, variables, error, inFunct);
    double intPart;
    if (arrayIndexValue.index() != 0 || modf(get<double>(arrayIndexValue), &intPart) != 0)
    {
        error = true;
        cout << "Runtime error: index is not an integer." << endl;
        exit(3);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    Value arrayExpression = evaluateAll(node->expression, variables, error, inFunct);
    if (arrayExpression.index() != 3)
    {
        error = true;
        cout << "Runtime error: not an array." << endl;
        exit(3);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    auto array = *get<shared_ptr<vector<Value>>>(arrayExpression);
    if (int(get<double>(arrayIndexValue)) >= int(array.size()))
    {
        error = true;
        cout << "Runtime error: index out of bounds." << endl;
        exit(3);
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
Value evaluateExpression(Node *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
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
        return evaluateFunctCall(fcNode, variables, error, inFunct);
    }
    if (ArrayLiteralNode *aLNode = dynamic_cast<ArrayLiteralNode *>(node))
    {
        // Node is an ArrayLiteralNode
        return evaluateArrayLiteral(aLNode, variables, error, inFunct);
    }
    if (ArrayAssignNode *aANode = dynamic_cast<ArrayAssignNode *>(node))
    {
        // Node is an ArrayAssignNode
        return evaluateArrayAssign(aANode, variables, error, inFunct, false);
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
            exit(3);
            return Value{numeric_limits<double>::quiet_NaN()};
        }
    }
    // Node is an operator but has no children
    else if (node->children.size() == 0)
    {
        // cout << node->token.text << endl;
        printErrorStatement(node->token, error);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    // Node is assignment operator
    else if (node->token.text == "=")
    {
        Value result = evaluateExpression(node->children[node->children.size() - 1], variables, error, inFunct);
        for (int i = int(node->children.size() - 2); i >= 0; i--)
        {
            if (ArrayAssignNode *aANode = dynamic_cast<ArrayAssignNode *>(node->children[i]))
            {
                // cout << "array assign found" << endl;
                Value array = evaluateArrayAssign(aANode, variables, error, inFunct, true);
                Value index = evaluateAll(aANode->arrayIndex, variables, error, inFunct);
                // printValue(array);
                // printValue(index);
                shared_ptr<vector<Value>> arrayPtr = get<shared_ptr<vector<Value>>>(array);
                (*arrayPtr)[int(get<double>(index))] = result;
            }
            else if (node->children[i]->token.type != IDENTIFIER)
            {
                error = true;
                cout << "Runtime error: invalid assignee." << endl;
                exit(3);

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
        Value result = evaluateExpression(node->children[0], variables, error, inFunct);
        if (holds_alternative<bool>(result))
        {
            // runtime error
            error = true;
            cout << "Runtime error: invalid operand type." << endl;
            exit(3);
            return  Value{numeric_limits<double>::quiet_NaN()};
        }
        // Iterate over the rest of the children to apply the operation.
        for (size_t i = 1; i < node->children.size(); i++)
        {
            if (holds_alternative<bool>(evaluateExpression(node->children[i], variables, error, inFunct)))
            {
                error = true;
                cout << "Runtime error: invalid operand type." << endl;
                exit(3);
                return Value{numeric_limits<double>::quiet_NaN()};
            }
            Token opToken = node->token;
            double resultDouble = get<double>(result);
            if (opToken.text == "+")
            {
                resultDouble += get<double>(evaluateExpression(node->children[i], variables, error, inFunct));
            }
            else if (opToken.text == "-")
            {
                resultDouble -= get<double>(evaluateExpression(node->children[i], variables, error, inFunct));
            }
            else if (opToken.text == "*")
            {
                resultDouble *= get<double>(evaluateExpression(node->children[i], variables, error, inFunct));
            }
            else if (opToken.text == "/")
            {
                // Check for division by zero.
                Value denominator = evaluateExpression(node->children[i], variables, error, inFunct);
                if (get<double>(denominator) != 0)
                {
                    resultDouble /= get<double>(denominator);
                }
                else
                {
                    error = true;
                    cout << "Runtime error: division by zero." << endl;
                    exit(3);
                    return Value{numeric_limits<double>::quiet_NaN()};
                }
            }
            else if (opToken.text == "%")
            {
                resultDouble = fmod(resultDouble, get<double>(evaluateExpression(node->children[i], variables, error, inFunct)));
            }
            else
            {
                // If the operation is unrecognized, print an error message.
                printErrorStatement(opToken, error);
                return Value{numeric_limits<double>::quiet_NaN()};
            }
            result = Value{resultDouble};
        }
        return result;
    }
    else if (node->token.type == COMPARATOR)
    {
        // Iterate over the rest of the children to apply the operation.
        Value result = evaluateExpression(node->children[0], variables, error, inFunct);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            Value childrenVal = evaluateExpression(node->children[i], variables, error, inFunct);
            Token opToken = node->token;
            if (opToken.text == "==" || opToken.text == "!=")
            {
                bool equality = true;
                // If different type or if same type but unequal, equality is false
                if (childrenVal.index() != result.index() ||
                    result != evaluateExpression(node->children[i], variables, error, inFunct))
                {
                    equality = false;
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
                    exit(3);
                    return Value{numeric_limits<double>::quiet_NaN()};
                }
                if (opToken.text == ">")
                {
                    result = Value{result > evaluateExpression(node->children[i], variables, error, inFunct)};
                }
                else if (opToken.text == "<")
                {
                    result = Value{result < evaluateExpression(node->children[i], variables, error, inFunct)};
                }
                else if (opToken.text == ">=")
                {
                    result = Value{result >= evaluateExpression(node->children[i], variables, error, inFunct)};
                }
                else if (opToken.text == "<=")
                {
                    // Check for division by zero.
                    result = Value{result <= evaluateExpression(node->children[i], variables, error, inFunct)};
                }
                else
                {
                    // If the operation is unrecognized, print an error message.
                    // cout << "unknown operator " << endl;
                    printErrorStatement(opToken, error);
                    return  Value{numeric_limits<double>::quiet_NaN()};
                }
            }
        }
        return result;
    }
    else if (node->token.type == LOGICAL)
    {
        // Iterate over the rest of the children to apply the operation.
        Value result = evaluateExpression(node->children[0], variables, error, inFunct);
        for (size_t i = 1; i < node->children.size(); i++)
        {
            Value childrenVal = evaluateExpression(node->children[i], variables, error, inFunct);
            if (holds_alternative<double>(childrenVal) || holds_alternative<double>(result))
            {
                error = true;
                cout << "Runtime error: invalid operand type." << endl;
                exit(3);
                return  Value{numeric_limits<double>::quiet_NaN()};
            }
            Token opToken = node->token;
            bool resultBool = get<bool>(result);
            if (opToken.text == "&")
            {
                resultBool = get<bool>(result) && get<bool>(evaluateExpression(node->children[i], variables, error, inFunct));
            }
            else if (opToken.text == "|")
            {
                resultBool = get<bool>(result) || get<bool>(evaluateExpression(node->children[i], variables, error, inFunct));
            }
            else
            {
                resultBool = get<bool>(result) != get<bool>(evaluateExpression(node->children[i], variables, error, inFunct));
            }
            result = Value{resultBool};
        }
        return result;
    }
    return  Value{numeric_limits<double>::quiet_NaN()};
}

Value len(Value array) {
    if (array.index() != 3) {
        cout << "Runtime error: not an array." << endl;
        exit(3);
    }
    shared_ptr<vector<Value>> arrayPtr = get<shared_ptr<vector<Value>>>(array);
    // printValue(Value{double((*arrayPtr).size())});
    return Value{double((*arrayPtr).size())};
}
Value push(Value array, Value value) {
    if (array.index() != 3) {
        cout << "Runtime error: not an array." << endl;
        exit(3);
    }
    shared_ptr<vector<Value>> arrayPtr = get<shared_ptr<vector<Value>>>(array);
    (*arrayPtr).push_back(value);
    return Value{nullptr};
}
Value pop(Value array) {
    if (array.index() != 3) {
        cout << "Runtime error: not an array." << endl;
        exit(3);
    }
    if (int(get<double>(len(array))) == 0) {
        cout << "Runtime error: underflow." << endl;
        exit(3);
    }
    shared_ptr<vector<Value>> arrayPtr = get<shared_ptr<vector<Value>>>(array);
    Value back = (*arrayPtr).back();
    (*arrayPtr).pop_back();
    return back;
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

    // Old statements tests
    // text = "print a = 132;\nprint b = 72;\n\nwhile a != 0 {\n  if a > b {\n    a = a % b;\n  }\n  else if b > a {\n    c = a;\n    a = b % a;\n    b = c;\n  }\n  else {}\n}\n\nprint b;"; // 132 72 12
    // text = "x = 57.5;\n\nif x > 90 {\n  print 4.0;\n}\nelse if x > 80 {\n  print 3.0;\n}\nelse if x > 70 {\n  print 2.0;\n}\nelse if x {\n  print 1.0;\n}\nelse {\n  print 0.0;\n}"; // not a bool

    // Functions tests
    // text = "def say_no() {\nprint false;\n}\ndef say_even(x) {\nprint x % 2 == 0;\n}\ndef say_equal(x, y, z) {\nprint x == y & y == z;\n}\nsay_even(14);\nsay_even(1 + 2);\nsay_no();\nsay_equal(4.0, 4, 4.00000);\nsay_equal(3, 3.00, 3.00001);"; // T F F T F
    // text = "z = 42;\n\n def foo(x, y) {\n def square(value) {\nreturn value * value;\n}\nprint square(x + y + z);\n\n}\nz = 108;\nf = foo;\nresult = f(1, 2);\nif result != null {\nprint result;\n}\n"; // 2025
    // text = "def foo(){}\ndef bar(){return;}\ndef\nbaz\n(\n)\n{\nreturn\nnull\n;\n}\n\nprint foo();\nprint bar();\nprint baz();"; // null null null
    // text = "def bar(x, y, z) {\n  print x*y + z;\n}\n\nbar(10, 11);\n"; // incorrect argument count
    // text = "def baz(arg) {\n  print [arg];\n}\n\nbaz(42);\nreturn;"; // unexpected return
    // text = "def foo(x) {\n  print x == x;\n}\n\nfoo = 1492;\nprint foo(99);"; // not a function

    // Array tests
    // text = "values = [null, null, null, null, null];\n\ndef set_index(index, value) {\n  values[index] = value;\n  values = [];\n}\n\nset_index(3, 1.3);\nset_index(1, true);\nset_index(2, [1, [2], 3]);\n\nprint values;"; // [null, true, [1, [2], 3], 1.3, null]
    // text = "def reverse(array) {\n  result = [];\n  while len(array) > 0 {\n    push(result, pop(array));\n  }\n\n  return result;\n}\n\nx = [1, 1, 2, 3, 5, 8, 13];\ny = reverse(x);\nprint x;\nprint y;"; // [13, 8, 5, 3, 2, 1, 1]
    // text = "S = [];\nK = [9, 3, 1, 1, 7];\nN = 14;\n\ndef swap(i, j) {\n\n   temp = S[i];\n    S[i] = S[j];\n    S[j] = temp;\n}\n\ndef init() {\n    i = 0;\n    while i < N {\n        push(S, i);\n        i = i + 1;\n    }\n}\n\ninit();\n\ni = 0;\nj = 0;\nwhile i < N {\n    j = (j + S[i] + K[i % len(K)]) % N;\n    swap(i, j);\n    i = i + 1;\n}\n\nprint(S);"; // [9, 13, 2, 7, 6, 4, 11, 5, 8, 12, 1, 10, 3, 0]
    // text = "def fib(n) {\n  if n <= 1 {\n    return 1;\n  }\n  else {\n    return fib(n - 1) + fib(n - 2);\n  }\n}\n\ndef fibvec(n) {\n  vec = [1, 1];\n\n  while len(vec) > n {\n    pop(vec);\n  }\n\n  while len(vec) < n {\n    push(vec, fib(len(vec)));\n  }\n\n\n return vec;\n}\n\nprint fibvec(0);\nprint fibvec(3);\n\ntop10 = fibvec(10);\nprint top10;"; // []  [1, 1, 2]  [1, 1, 2, 3, 5, 8, 13, 21, 34, 55]
    // text = "x = [1, 2];\nprint [1, 2] == x;";

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
        bool inFunct = false;
        evaluateAll(trees[i], variables, error, inFunct);
    }
    for (size_t i = 0; i < trees.size(); i++)
    {
        deleteNodeAll(trees[i]);
    }
    return 0;
}