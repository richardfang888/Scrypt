#include "lib/scrypt.hpp"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <limits>
#include <cmath>

using namespace std;

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
    // cout << "evaluateIfElse" << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    Value condResult = evaluateExpression(node->condition, variables, error, inFunct);
    if (holds_alternative<double>(condResult))
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
    if (holds_alternative<double>(condResult))
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
            if (holds_alternative<double>(condResult))
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
    if (holds_alternative<double>(result) && !isnan(get<double>(result)))
    {
        cout << get<double>(result) << endl;
    }
    else if (holds_alternative<bool>(result))
    {
        if (get<bool>(result)) {
            cout << "true" << endl;
        }
        else {
            cout << "false" << endl;
        }
    }
    else if (result.index() == 2)
    {
        cout << "null" << endl;
    }
    // dummy return -> can make it void if needed
    return Value{numeric_limits<double>::quiet_NaN()};
}

// Evaluates a return statement.
Value evaluateReturn(ReturnNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    // cout << "eval return" << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    if (!node->expression)
    {
        return Value{nullptr};
    }
    Value result = evaluateExpression(node->expression, variables, error, inFunct);
    // cout << "return value: " << get<double>(result) << endl;

    // set the value of the function call to return
    return result;
}

// Evaluates a function definition.
Value evaluateFunctDef(FunctDefNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    // add function to defined functions
    Function func;
    func.function = node;
    func.functVariables = variables;
    variables[node->functname.text] = Value{make_shared<Function>(func)};
    // cout << "Function " << node->functname.text << " defined." << endl;

    // dummy return
    inFunct = inFunct;
    return Value{numeric_limits<double>::quiet_NaN()};
}

// Evaluates a function call.
Value evaluateFunctCall(FunctCallNode *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
    // cout << "eval funct call" << endl;
    if (!node || error)
    {
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    inFunct = true;
    // check if function exists
    // cout << "Function " << node->functname.text << " called." << endl;
    auto iter = variables.find(node->functname.text);
    if (iter == variables.end()) 
    {
        error = true;
        // function undefined
        cout << "Runtime error: not a function." << endl;
        exit(3);
        return Value{numeric_limits<double>::quiet_NaN()};
    }

    Value functionValue = iter->second;
    if (!holds_alternative<shared_ptr<Function>>(functionValue))
    {
        error = true;
        // function undefined
        cout << "Runtime error: not a function." << endl;
        exit(3);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    shared_ptr<Function> function = get<shared_ptr<Function>>(functionValue);
    
    // check if number of arguments matches
    if (node->arguments.size() != function->function->params.size()) {
        cout << "Runtime error: incorrect argument count." << endl;
        exit(3);
        return Value{numeric_limits<double>::quiet_NaN()};
    }

    // evaluate arguments
    vector<Value> evaluatedArguments;
    for (Node *argument : node->arguments)
    {
        // cout << "Argument: " << argument->token.text << endl;
        evaluatedArguments.push_back(evaluateAll(argument, variables, error, inFunct));
        inFunct = true;
        // Value result = evaluateAll(argument, variables, error, inFunct);
        // cout << "Evaluated argument: " << get<double>(result) << endl;
    }
    
    if (!function->function) {
        error = true;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    // check if arguments match
    if (evaluatedArguments.size() != function->function->params.size()) {
        error = true;
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    // create new scope for function call
    for (size_t i = 0; i < function->function->params.size(); i++)
    {
        function->functVariables[function->function->params[i]] = evaluatedArguments[i];
    }
    // Evaluate the statements within the function scope
    for (Node *statement : function->function->statements)
    {
        Value result = evaluateAll(statement, function->functVariables, error, inFunct);
        if (error) {
            // Handle errors that occurred during evaluation
            return Value{numeric_limits<double>::quiet_NaN()};
        }
        // cout << result.index() << endl;
        if (result.index() != 0 || !isnan(get<double>(result))) {
            // cout << "actual return " << get<double>(result) << endl;
            return result;
        }
    }
    inFunct = false;
    return Value{nullptr};
}


// Evaluates an expression given the root of the expression's AST.
Value evaluateExpression(Node *node, unordered_map<string, Value> &variables, bool &error, bool &inFunct)
{
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
        // cout << "no children" << endl;
        printErrorStatement(node->token, error);
        return Value{numeric_limits<double>::quiet_NaN()};
    }
    // Node is assignment operator
    else if (node->token.text == "=")
    {
        Value result = evaluateExpression(node->children[node->children.size() - 1], variables, error, inFunct);
        for (int i = int(node->children.size() - 2); i >= 0; i--)
        {
            if (node->children[i]->token.type != IDENTIFIER)
            {
                error = true;
                cout << "Runtime error: invalid asignee." << endl;
                exit(3);

                return Value{numeric_limits<double>::quiet_NaN()};
            }
            variables[node->children[i]->token.text] = result;
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
                return  Value{numeric_limits<double>::quiet_NaN()};
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
    // text = "print a = 132;\nprint b = 72;\n\nwhile a != 0 {\n  if a > b {\n    a = a % b;\n  }\n  else if b > a {\n    c = a;\n    a = b % a;\n    b = c;\n  }\n  else {}\n}\n\nprint b;";
    // text = "x = 57.5;\n\nif x > 90 {\n  print 4.0;\n}\nelse if x > 80 {\n  print 3.0;\n}\nelse if x > 70 {\n  print 2.0;\n}\nelse if x {\n  print 1.0;\n}\nelse {\n  print 0.0;\n}";

    // Functions tests
    // text = "def say_no() {\nprint false;\n}\ndef say_even(x) {\nprint x % 2 == 0;\n}\ndef say_equal(x, y, z) {\nprint x == y & y == z;\n}\nsay_even(14);\nsay_even(1 + 2);\nsay_no();\nsay_equal(4.0, 4, 4.00000);\nsay_equal(3, 3.00, 3.00001);";
    // text = "z = 42;\n\n def foo(x, y) {\n def square(value) {\nreturn value * value;\n}\nprint square(x + y + z);\n\n}\nz = 108;\nf = foo;\nresult = f(1, 2);\nif result != null {\nprint result;\n}\n";
    // text = "def foo(){}\ndef bar(){return;}\ndef\nbaz\n(\n)\n{\nreturn\nnull\n;\n}\n\nprint foo();\nprint bar();\nprint baz();";
    // text = "def bar(x, y, z) {\n  print x*y + z;\n}\n\nbar(10, 11);\n";
    // text = "def baz(arg) {\n  print arg;\n}\n\nreturn baz(42);";
    // text = "def foo(x) {\n  print x == x;\n}\n\nfoo = 1492;\nprint foo(99);";

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