#pragma once
# include <string>
# include <vector>

using namespace std;

enum TokenType {
    // operators
    PLUS,
    MINUS,
    TIMES,
    DIVIDES,
    ASSIGN,

    // identifiers
    IDENTIFIER,

    // numbers
    FLOAT,
    DOT,

    // delimiters
    NEWLINE,
    WHITESPACE,
    LEFT_PAREN,
    RIGHT_PAREN,
    END,

    // other unexpected tokens (syntax error)
    OTHER
};

struct Token {
    enum TokenType type;
    string text;
    int length;
    int lineNumber;
    int columnNumber;
};
