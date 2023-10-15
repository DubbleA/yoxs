#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "../lexer/lexer.hpp"
#include "../token/token.hpp"
#include "../ast/ast.hpp"

enum Precedence {
    LOWEST = 1,
    EQUALS,
    LESSGREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL
};

extern std::unordered_map<TokenType, Precedence> precedences;

class Parser {
public:
    Parser(Lexer& l);
    ~Parser();
    
    std::vector<std::string> errors;
    std::unique_ptr<Program> ParseProgram();

private:
    Lexer* lexer;
    Token curToken;
    Token peekToken;

    typedef Expression*(Parser::*prefixParseFn)();
    typedef Expression*(Parser::*infixParseFn)(Expression*);

    std::unordered_map<TokenType, prefixParseFn> prefixParseFns;
    std::unordered_map<TokenType, infixParseFn> infixParseFns;

    void nextToken();
    bool curTokenIs(TokenType t);
    bool peekTokenIs(TokenType t);
    bool expectPeek(TokenType t);
    
    void registerPrefix(TokenType type, prefixParseFn fn);
    void registerInfix(TokenType type, infixParseFn fn);

    // Parsing functions here...
};

#endif // PARSER_H
