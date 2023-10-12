// token.hpp
// header file for token struct

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

    // numbers
    FLOAT,

    // delimiters
    LEFT_PAREN,
    RIGHT_PAREN,
    END
};

struct Token {
    enum TokenType type;
    string text;
    int length;
    int lineNumber;
    int columnNumber;
};
