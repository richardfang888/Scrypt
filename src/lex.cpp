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
    currToken.lineNumber = 1;
    currToken.columnNumber = 1;

    for (char c : input) {
        // handle newline
        switch (c) {
            // handle delimiters
            case 'E':  // unsure about END token
                createToken(currToken, tokens, END, "END", 3);
                return tokens;
            case '\n':
                currToken.lineNumber++;
                currToken.columnNumber = 1;
                break;
            case ' ':
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
            case '.':
                if (currToken.type == FLOAT) {
                    currToken.text += c;
                    currToken.length++;
                    currToken.lineNumber = tokens.back().lineNumber; 
                    tokens.pop_back();
                    createToken(currToken, tokens, FLOAT, currToken.text, currToken.length);
                }
                else {
                    createToken(currToken, tokens, FLOAT, string(1, c), 1);
                }
                break;
        }
    }

    return tokens;
}

void printTokens(vector<Token> &tokens) {
    int maxLineNumWidth = 0;
    int maxColNumWidth = 0;

    for (const Token& token : tokens) {
        int lineNumWidth = to_string(token.lineNumber).length();
        int colNumWidth = to_string(token.columnNumber).length();

        maxLineNumWidth = max(maxLineNumWidth, lineNumWidth);
        maxColNumWidth = max(maxColNumWidth, colNumWidth);
    }

    for (const Token& token : tokens) {
        cout << right << setw(maxLineNumWidth) << token.lineNumber << " "
                << right << setw(maxColNumWidth) << token.columnNumber << " "
                << token.text << endl;
    }
}


int main(int argc, const char** argv) {
    string input;
    getline(cin, input);
    vector<Token> tokens = readTokens(input);
    printTokens(tokens);
    return 0;
}
