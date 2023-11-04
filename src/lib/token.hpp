#pragma once
# include <string>
# include <vector>

using namespace std;

enum TokenType {
    OPERATOR, // +, -, *, /, =, %
    COMPARATOR, // ==, !=, >, <, >=, <=
    LOGICAL, // &, |, ^
    IDENTIFIER, // variable
    FLOAT, // number
    BOOLEAN, // true, false
    WHITESPACE,
    LEFT_PAREN,
    RIGHT_PAREN,
    END,

    // other unexpected tokens (syntax error)
    OTHER
};

struct Token {
    enum TokenType type{WHITESPACE};
    string text;
    int length{0};
    int lineNumber;
    int columnNumber;
};
