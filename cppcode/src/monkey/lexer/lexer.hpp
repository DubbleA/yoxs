#ifndef LEXER_H
#define LEXER_H

#include <string>
#include "monkey/token/token.hpp"

class Lexer {
private:
    std::string input;
    int position;         // current position in input (points to current char)
    int readPosition;     // current reading position in input (after current char)
    char ch;              // current char under examination

    void readChar();
    char peekChar() const;
    std::string readIdentifier();
    std::string readNumber();
    void skipWhitespace();
    static bool isLetter(char ch);
    static bool isDigit(char ch);
    static Token newToken(TokenType tokenType, char ch);

public:
    Lexer(const std::string& input);
    Token NextToken();
};

#endif // LEXER_H
