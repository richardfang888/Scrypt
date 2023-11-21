#include "lex.hpp"
#include "cmath"
#include <iomanip>
#include <iostream>
#include <algorithm>

using namespace std;

void finishToken(Token &currToken, vector<Token> &tokens) {
    if (currToken.length == 0) {
        return;
    }
    if (currToken.type == IDENTIFIER)
    {
        if (currToken.text == "if" || currToken.text == "print" || currToken.text == "while" 
                || currToken.text == "else" || currToken.text == "return" || currToken.text == "def")
        {
            currToken.type = KEYWORD;
        }
        if (currToken.text == "true" || currToken.text == "false")
        {
            currToken.type = BOOLEAN;
        }
        if (currToken.text == "null")
        {
            currToken.type = NULLVAL;
        }
    }
    if (currToken.type != WHITESPACE)
    {
        tokens.push_back(currToken);
    }
    currToken.columnNumber += currToken.length;
    currToken.length = 0;
    currToken.type = WHITESPACE;
    currToken.text = "";
}

// Converts an input string into a list of tokens representing its content
vector<Token> readTokens(string &input)
{
    vector<Token> tokens;
    Token currToken;
    currToken.lineNumber = 1;
    currToken.columnNumber = 1;

    for (int i = 0; i < int(input.size()); i++)
    {
        char c = input[i];

        // handle delimiters
        switch (c)
        {
        case '\n':
            finishToken(currToken, tokens);
            currToken.lineNumber++;
            currToken.columnNumber = 1;
            break;
        case ' ':
        case '\t':
            finishToken(currToken, tokens);
            currToken.columnNumber++;
            break;

        case '(':
        case ')':
        case '{':
        case '}':
        case ';':
        case ',':
            finishToken(currToken, tokens);
            currToken.text += c;
            currToken.length++;
            if (c == '(')
            {
                currToken.type = LEFT_PAREN;
            }
            else if (c == ')')
            {
                currToken.type = RIGHT_PAREN;
            }
            else if (c == '{')
            {
                currToken.type = LEFT_BRACE;
            }
            else if (c == '}')
            {
                currToken.type = RIGHT_BRACE;
            }
            else if (c == ';')
            {
                currToken.type = SEMICOLON;
            }
            else if (c == ',')
            {
                currToken.type = COMMA;
            }
            finishToken(currToken, tokens);
            break;

        // handle operators
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
            finishToken(currToken, tokens);
            currToken.text += c;
            currToken.length++;
            currToken.type = OPERATOR;
            finishToken(currToken, tokens);
            break;
        
        // handle logical operators
        case '&':
        case '|':
        case '^':
            finishToken(currToken, tokens);
            currToken.text += c;
            currToken.length++;
            currToken.type = LOGICAL;
            finishToken(currToken, tokens);
            break;

        // handle comparators
        case '!':
        case '>':
        case '<':
            finishToken(currToken, tokens);
            currToken.type = COMPARATOR;
            currToken.text += c;
            currToken.length++;
            break;
        
        // either operator or comparator
        case '=':
            // handle == case
            if (currToken.text == "=") {
                currToken.type = COMPARATOR;
            }
            // handle !=, >=, <=, == comparator cases
            if (currToken.type == COMPARATOR) {
                currToken.text += c;
                currToken.length++;
                finishToken(currToken, tokens);
            }
            // default assingment operator case
            else {
                finishToken(currToken, tokens);
                currToken.type = OPERATOR;
                currToken.text += c;
                currToken.length++;
            }
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
            if (currToken.type != FLOAT && currToken.type != IDENTIFIER) {
                finishToken(currToken, tokens);
                currToken.type = FLOAT;
            }
            currToken.text += c;
            currToken.length++;
            break;

        case '.':
            if (currToken.type != FLOAT || currToken.text.find(".") != string::npos)
            {
                finishToken(currToken, tokens);
                LexError(tokens, currToken.lineNumber, currToken.columnNumber);
                vector<Token> empty;
                return empty;
            }
            else if (i == int(input.size()) - 1  || !(input[i+1] >= '0' && input[i+1] <= '9'))
            {
                currToken.length++;
                finishToken(currToken, tokens);
                LexError(tokens, currToken.lineNumber, currToken.columnNumber);
                vector<Token> empty;
                return empty;
            }
            currToken.text += c;
            currToken.length++;
            break;

        // handle identifiers
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
            if (currToken.type != IDENTIFIER)
            {
                finishToken(currToken, tokens);
                currToken.type = IDENTIFIER;
            }
            currToken.text += c;
            currToken.length++;
            break;

        // handle unknown tokens (syntax error)
        default:
            if (!isspace(c))
            {
                finishToken(currToken, tokens);
                LexError(tokens, currToken.lineNumber, currToken.columnNumber);
                vector<Token> empty;
                return empty;
            }
        }
    }
    finishToken(currToken, tokens);
    // post-processing
    if ((!tokens.empty() && tokens.back().text == "error" ) || (tokens.size() > 2 && tokens[tokens.size() - 2].text == "error")) 
    {
        vector<Token> empty;
        return empty;
    }
    else if (tokens.empty() || tokens.back().type != END)
    {   
        currToken.type = END;
        currToken.text = "END";
        currToken.length++;
        finishToken(currToken, tokens);
    }
    
    return tokens;
}

// Checks for lexical errors in the given list of tokens
void LexError(vector<Token> &tokens, int lineNumber, int columnNumber)
{
    cout << "Syntax error on line " << lineNumber << " column "
            << columnNumber << "." << endl;
    tokens.push_back({OTHER, "error", 0, lineNumber, columnNumber});
}

void printTokens(vector<Token> &tokens)
{
    for (const Token &token : tokens)
    {
        cout << setw(4) << token.lineNumber << setw(5) << token.columnNumber << "  " << token.text << endl;
    }
}