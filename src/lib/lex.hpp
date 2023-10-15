# include "token.hpp"

void createToken(Token &currToken, vector<Token> &tokens, TokenType type, const string& text, int length);

vector<Token> readTokens(string &input);

void printTokens(vector<Token> &tokens);


