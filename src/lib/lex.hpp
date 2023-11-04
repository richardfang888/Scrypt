#include "token.hpp"

void finishToken(Token &currToken, vector<Token> &tokens);

vector<Token> readTokens(string &input);

void printTokens(vector<Token> &tokens);

void LexError(vector<Token> &tokens, int lineNumber, int columnNumber);

// void CalcLexError(int lineNumber, int columnNumber);