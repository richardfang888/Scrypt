#include "lex.hpp"
#include <iomanip>
#include <iostream>
#include <algorithm>

using namespace std;

// Helper function to create and push tokens
void createToken(Token &currToken, vector<Token> &tokens, TokenType type, const string &text, int length)
{
    currToken.type = type;
    currToken.text = text;
    currToken.length = length;
    tokens.push_back(currToken);
    currToken.columnNumber += length;
}

// Converts an input string into a list of tokens representing its content
vector<Token> readTokens(string &input)
{
    vector<Token> tokens;
    Token currToken;
    currToken.type = OTHER;
    currToken.lineNumber = 1;
    currToken.columnNumber = 1;

    for (char c : input)
    {
        if (currToken.type == DOT && !(c >= '0' && c <= '9'))
        {
            createToken(currToken, tokens, OTHER, "error", 1);
            return tokens;
        }

        // handle delimiters
        switch (c)
        {
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
        case '=':
            createToken(currToken, tokens, ASSIGN, "=", 1);
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
            if (currToken.type == IDENTIFIER)
            {
                currToken.text += c;
                currToken.length++;
                currToken.columnNumber = tokens.back().columnNumber;
                tokens.pop_back();
                createToken(currToken, tokens, IDENTIFIER, currToken.text, currToken.length);
            }
            else if (currToken.type == FLOAT || currToken.type == DOT)
            {
                currToken.text += c;
                currToken.length++;
                currToken.columnNumber = tokens.back().columnNumber;
                tokens.pop_back();
                createToken(currToken, tokens, FLOAT, currToken.text, currToken.length);
            }
            else
            {
                createToken(currToken, tokens, FLOAT, string(1, c), 1);
            }
            break;
        case '.':
            if (currToken.type != FLOAT || currToken.text.find(".") != string::npos)
            {
                createToken(currToken, tokens, OTHER, "error", 1);
                return tokens;
            }
            else
            {
                currToken.text += c;
                currToken.length++;
                currToken.columnNumber = tokens.back().columnNumber;
                tokens.pop_back();
                createToken(currToken, tokens, DOT, currToken.text, currToken.length);
            }
            break;

        //handle identifiers 
        case '_':
        case 'a':
        case 'A':
        case 'b':
        case 'B':
        case 'c':
        case 'C':
        case 'd':
        case 'D':
        case 'e':
        case 'E':
        case 'f':
        case 'F':
        case 'g':
        case 'G':
        case 'h':
        case 'H':
        case 'i':
        case 'I':
        case 'j':
        case 'J':
        case 'k':
        case 'K':
        case 'l':
        case 'L':
        case 'm':
        case 'M':
        case 'n':
        case 'N':
        case 'o':
        case 'O':
        case 'p':
        case 'P':
        case 'q':
        case 'Q':
        case 'r':
        case 'R':
        case 's':
        case 'S':
        case 't':
        case 'T':
        case 'u':
        case 'U':
        case 'v':
        case 'V':
        case 'w':
        case 'W':
        case 'x':
        case 'X':
        case 'y':
        case 'Y':
        case 'z':
        case 'Z':
            if (currToken.type == IDENTIFIER)
            {
                currToken.text += c;
                currToken.length++;
                currToken.columnNumber = tokens.back().columnNumber;
                tokens.pop_back();
                createToken(currToken, tokens, IDENTIFIER, currToken.text, currToken.length);
            }
            else
            {
                if(currToken.type != FLOAT)
                {
                    createToken(currToken, tokens, IDENTIFIER, string(1, c), 1);
                }
                else{
                    createToken(currToken, tokens, OTHER, "error", 1);
                    return tokens;
                }
            }
            break;

        // handle unknown tokens (syntax error)
        default:
            if (!isspace(c))
            {
                createToken(currToken, tokens, OTHER, string(1, c), 1);
                return tokens;
            }
        }
    }

    // post-processing
    if (!tokens.empty() && tokens.back().type == DOT)
    {
        createToken(currToken, tokens, OTHER, "error", 1);
    }
    else if (tokens.empty() || tokens.back().type != TokenType::END)
    {
        createToken(currToken, tokens, END, "END", 1);
    }

    return tokens;
}

// Checks for lexical errors in the given list of tokens
void checkLexErrors(vector<Token> &tokens)
{
    if (tokens.back().type == TokenType::OTHER)
    {
        cout << "Syntax error on line " << tokens.back().lineNumber << " column "
             << tokens.back().columnNumber << "." << endl;
        exit(1);
    }
}

void checkCalcLexErrors(vector<Token> &tokens)
{
    if (tokens.back().type == TokenType::OTHER)
    {
        cout << "Syntax error on line " << tokens.back().lineNumber << " column "
             << tokens.back().columnNumber << "." << endl;
    }
}

void printTokens(vector<Token> &tokens)
{
    for (const Token &token : tokens)
    {
        cout << setw(4) << token.lineNumber << setw(5) << token.columnNumber << "  " << token.text << endl;
    }
}