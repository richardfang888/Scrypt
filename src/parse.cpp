# include "lib/parse.hpp"
# include <iostream>

using namespace std;

AST::AST(const vector<Token>& tokens) {
    int index = 0;
    root = buildTree(tokens, index, tokens.size() - 1);
}

Node* AST::buildTree(const vector<Token>& tokens, int index, int eindex) {
    if (index >= tokens.size()) {
        std::cerr << "Error: Invalid S expression. Unexpected end of input." << std::endl;
        return nullptr;
    }

    Token token = tokens[index];
    
    // If the current token is a float -> it is a leaf node, end recursion
    if (token.type == FLOAT) {
        Node* node = new Node();
        node->token = token;
        return node;
    }
    // If the current token is a left parenthesis -> nested expression, recurse
    else if (token.type == LEFT_PAREN) {
        index += 1;
        // Expression must start with an operator
        if (tokens[index].type != PLUS && tokens[index].type != MINUS && tokens[index].type != TIMES && tokens[index].type != DIVIDES) {
            std::cerr << "Error: Invalid S expression. Unexpected token '" << tokens[index].text << "'" << std::endl;
            return nullptr;
        }
        // Expression must end with a right parenthesis
        if (tokens[eindex].type != RIGHT_PAREN) {
            std::cerr << "Error: Invalid S expression. Missing or misplaced right parenthesis." << std::endl;
            return nullptr;
        }
        // Create the root node of this subtree (operator)
        Node* node = new Node();
        // Recursively build the children for the root
        while (index < eindex) {
            // Fix this recursion so that tokens are not added multiple times and left and right parenthesis not added
            Node* child = buildTree(tokens, index, eindex);
            if (child) {
                node->children.push_back(child);
            }
        }

        if (tokens[index].type != RIGHT_PAREN) {
            std::cerr << "Error: Invalid S expression. Missing or misplaced right parenthesis." << std::endl;
            delete node;
            return nullptr;
        }

        index++; // Move to the next token after the right parenthesis
        return node;
    } else {
        std::cerr << "Error: Invalid S expression. Unexpected token '" << token.text << "'" << std::endl;
        return nullptr;
    }
}

double AST::evaluateAST() {
    if (!root) {
        return 0;
    }
    return evaluate(root);
}

double AST::evaluate(Node *node) {
    if (node->token.type == FLOAT) {
        return stoi(node->token.text);
    } else if (node->children.size() == 0) {
        cerr << "Error: Invalid AST node." << endl;
        return 0; // Or handle the error case as appropriate for your application
    } else {
        int result = evaluate(node->children[0]);
        for (size_t i = 1; i < node->children.size(); i++) {
            Token opToken = node->token;
            if (opToken.type == PLUS) {
                result += evaluate(node->children[i]);
            } else if (opToken.type == MINUS) {
                result -= evaluate(node->children[i]);
            } else if (opToken.type == TIMES) {
                result *= evaluate(node->children[i]);
            } else if (opToken.type == DIVIDES) {
                int denominator = evaluate(node->children[i]);
                if (denominator != 0) {
                    result /= denominator;
                } else {
                    cerr << "Error: Division by zero." << endl;
                    return 0; // Or handle the error case as appropriate for your application
                }
            }
        }
        return result;
    }
}
Node* AST::getRoot() const {
    return root;
}

void AST::printInfix(){
}


int main(int argc, const char** argv) {
    string input;
    string text;
    vector<Token> tokens;

    // while (getline(cin, input)) {
    //     text += input;
    //     if (!cin.eof()) {
    //         text += '\n';
    //     }
    // }

    text = "(+(-2 4.444 ) 32(* 5 13.45))";
    tokens = readTokens(text);
    printTokens(tokens);
    
    AST ast(tokens);
    cout << ast.evaluateAST() << endl;
    
    return 0;
}
