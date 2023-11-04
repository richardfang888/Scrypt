#include "token.hpp"
#include <stdexcept>

void finishToken(Token &currToken, vector<Token> &tokens);

vector<Token> readTokens(string &input);

void printTokens(vector<Token> &tokens);

// class lexer_error : public std::exception
// {
// public:
//     lexer_error(const std::string& message) : message(message) {}
    
//     const char* what() const noexcept override
//     {
//         return message.c_str();
//     }
    
// private:
//     std::string message;
// };

void LexError(vector<Token> &tokens, int lineNumber, int columnNumber);

