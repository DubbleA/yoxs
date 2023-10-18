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
    EQUALS, // ==
    LESSGREATER, // > or <
    SUM, // + 
    PRODUCT, // * 
    PREFIX, //-X or !X
    CALL //myFunction(X)
};

extern std::unordered_map<TokenType, Precedence> precedences;

class Parser {
public:
    Parser(Lexer& l);
    ~Parser();
    
    std::unique_ptr<Program> ParseProgram();
    std::vector<std::string> Errors() const; 

private:
    Lexer* lexer;
    Token curToken;
    Token peekToken;
    std::vector<std::string> errors;

    using prefixParseFn = std::function<std::unique_ptr<Expression>(void)>;
    using infixParseFn = std::function<std::unique_ptr<Expression>(std::unique_ptr<Expression>)>;

    std::unordered_map<TokenType, prefixParseFn> prefixParseFns;
    std::unordered_map<TokenType, infixParseFn> infixParseFns;

    void registerPrefix(TokenType type, prefixParseFn fn);
    void registerInfix(TokenType type, infixParseFn fn);

    void nextToken();
    bool curTokenIs(TokenType t) const;
    bool peekTokenIs(TokenType t) const;
    bool expectPeek(TokenType t);
    void peekError(TokenType t);
    void noPrefixParseFnError(TokenType t);
    int peekPrecedence() const;
    int curPrecedence() const;
    

    // Parsing functions here...
};

#endif // PARSER_H
