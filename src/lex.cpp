# include "lib/lex.hpp"
# include <iomanip>
# include <iostream>
# include <algorithm>

using namespace std;

// Helper function to create and push tokens
void createToken(Token &currToken, vector<Token> &tokens, TokenType type, const string &text, int length) {
    currToken.type = type;
    currToken.text = text;
    currToken.length = length;
    tokens.push_back(currToken);
    currToken.columnNumber += length;
}

vector<Token> readTokens(string &input) {
    vector<Token> tokens;
    Token currToken;
    currToken.type = OTHER;
    currToken.lineNumber = 1;
    currToken.columnNumber = 1;

    for (char c : input) {
        if (currToken.type == DOT && !(c >= '0' && c <= '9')) { 
            createToken(currToken, tokens, OTHER, string(1, c), 1);
            return tokens;
        }

        // handle delimiters
        switch (c) {
            case '\n':
                currToken.type = NEWLINE;
                currToken.lineNumber++;
                currToken.columnNumber = 1;
                break;
            case ' ':
            case '\t':
                currToken.type = WHITESPACE;
                currToken.columnNumber++;
                break;
            case '(':
                createToken(currToken, tokens, LEFT_PAREN, "(", 1);
                break;
            case ')':
                createToken(currToken, tokens, RIGHT_PAREN, ")", 1);
                break;

            // handle operators
            case '+':
                createToken(currToken, tokens, PLUS, "+", 1);
                break;
            case '-':
                createToken(currToken, tokens, MINUS, "-", 1);
                break;
            case '*':
                createToken(currToken, tokens, TIMES, "*", 1);
                break;
            case '/':
                createToken(currToken, tokens, DIVIDES, "/", 1);
                break;
            
            // handle numbers
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if (currToken.type == FLOAT || currToken.type == DOT) {
                    currToken.text += c;
                    currToken.length++;
                    currToken.columnNumber = tokens.back().columnNumber; 
                    tokens.pop_back();
                    createToken(currToken, tokens, FLOAT, currToken.text, currToken.length);
                }
                else {
                    createToken(currToken, tokens, FLOAT, string(1, c), 1);
                }
                break;
            case '.':
                if (currToken.type != FLOAT || currToken.text.find(".") != string::npos) {
                    createToken(currToken, tokens, OTHER, string(1, c), 1);
                    return tokens;
                }
                else {
                    currToken.text += c;
                    currToken.length++;
                    currToken.columnNumber = tokens.back().columnNumber; 
                    tokens.pop_back();
                    createToken(currToken, tokens, DOT, currToken.text, currToken.length);
                }
                break;
            
            // handle unknown tokens (syntax error)
            default:
                if (!isspace(c)) {
                    createToken(currToken, tokens, OTHER, string(1, c), 1);
                    return tokens;
                }
        }
    }

    if (tokens.back().type == DOT) {
        createToken(currToken, tokens, OTHER, ".", 1);
        cout << "trailing";
    }

    if (tokens.back().type != TokenType::END) {
        createToken(currToken, tokens, END, "END", 1);
    }

    return tokens;
}

void printTokens(vector<Token> &tokens) {
    if (tokens.back().type == TokenType::OTHER) {
        cout << "Syntax error on line " << tokens.back().lineNumber << " column " 
             << tokens.back().columnNumber << "." << endl;
        exit(1);
    }

    for (const Token& token : tokens) {
        cout << setw(4) << token.lineNumber << setw(5) << token.columnNumber << "  " << token.text << endl;
    }
}


int main(int argc, const char** argv) {
    string input;
    string text;
    vector<Token> tokens;

    while (getline(cin, input)) {
        text += input;
        if (!cin.eof()) {
            text += '\n';
        }
    }

    tokens = readTokens(text);
    printTokens(tokens);

    return 0;
}
