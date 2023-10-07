#include "token.hpp"

Token::Token(TokenType type, const std::string& literal) : Type(type), Literal(literal) {}

static std::unordered_map<std::string, TokenType> keywords = {
    {"fn", TokenType::FUNCTION},
    {"let", TokenType::LET},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"return", TokenType::RETURN}
};

TokenType LookupIdent(const std::string& ident) {
    auto it = keywords.find(ident);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENT;
}
