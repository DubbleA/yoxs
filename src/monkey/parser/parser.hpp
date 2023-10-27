#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <sstream>
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

    std::vector<std::string> Errors() const; 
    std::unique_ptr<Program> ParseProgram();

private:
    Lexer* lexer;
    Token curToken;
    Token peekToken;
    //curToken and peekToken act exactly like the two “pointers” our 
    //lexer has: position and readPosition. But instead of pointing 
    //to a character in the input, they point to the current and the 
    //next token. Both are important: we need to look at the curToken, 
    //which is the current token under examination, to decide what to 
    //do next, and we also need peekToken for this decision if 
    //curToken doesn’t give us enough information.
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

    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<LetStatement> parseLetStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();

    std::unique_ptr<Expression> parseExpression(Precedence pVal);

    std::unique_ptr<Identifier> parseIdentifier();
    std::unique_ptr<IntegerLiteral> parseIntegerLiteral();

    std::unique_ptr<PrefixExpression> parsePrefixExpression();
    std::unique_ptr<InfixExpression> parseInfixExpression(std::unique_ptr<Expression> left);
    std::unique_ptr<Boolean> parseBoolean();
    std::unique_ptr<Expression> parseGroupedExpression();
    std::unique_ptr<IfExpression> parseIfExpression();
    std::unique_ptr<BlockStatement> parseBlockStatement();
    std::unique_ptr<FunctionLiteral> parseFunctionLiteral();
    std::vector<std::unique_ptr<Identifier>> parseFunctionParameters();     
    std::unique_ptr<CallExpression> parseCallExpression(std::unique_ptr<Expression> function);
    std::vector<std::unique_ptr<Expression>> parseCallArguments();

};

#endif // PARSER_H
