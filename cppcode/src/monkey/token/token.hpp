#ifndef TOKEN_H
#define TOKEN_H
#include <string>
#include <unordered_map>

enum class TokenType {
    ILLEGAL,
    EOF_TOKEN,  // EOF is a macro in C++, so renaming to avoid conflicts

    // Identifiers + literals
    IDENT,  // add, foobar, x, y, ...
    INT,    // 1343456

    // Operators
    ASSIGN, 
    PLUS,
    MINUS,
    BANG,
    ASTERISK,
    SLASH,
    LT,
    GT,
    EQ,
    NOT_EQ,

    // Delimiters
    COMMA,
    SEMICOLON,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    // Keywords
    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELSE,
    RETURN
};

class Token {
public:
    TokenType Type;
    std::string Literal;
    Token() = default;
    Token(TokenType type, const std::string& literal);
};

TokenType LookupIdent(const std::string& ident);

#endif // TOKEN_H