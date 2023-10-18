#include "parser.hpp"
#include "../token/token.hpp"
#include <sstream> //for std::ostringstream

std::unordered_map<TokenType, int> precedences = {
    { TokenType::EQ, EQUALS },
    { TokenType::NOT_EQ, EQUALS },
    { TokenType::LT, LESSGREATER },
    { TokenType::GT, LESSGREATER},
    { TokenType::PLUS, SUM},
    { TokenType::MINUS, SUM},
    { TokenType::SLASH, PRODUCT},
    { TokenType::ASTERISK, PRODUCT},
    { TokenType::LPAREN, CALL}

};

Parser::Parser(Lexer& l) : lexer(&l) {
    // Initialization code here...
    nextToken();
    nextToken();
    // TODO: Other initializations, similar to the Go code...
}

Parser::~Parser() {
    // Any cleanup if required...
}

std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();
    while(!curTokenIs(TokenType::EOF)) {

    }
    return program;
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = lexer->NextToken();
}

bool Parser::curTokenIs(TokenType t) const {
    return curToken.Type == t;
}

bool Parser::peekTokenIs(TokenType t) const {
    return peekToken.Type == t;
}

bool Parser::expectPeek(TokenType t) {
    if(peekTokenIs(t)) {
        nextToken();
        return true;
    } else{
        peekError(t);
        return false;
    }
}

void Parser::peekError(TokenType t) {
    std::ostringstream oss; 
    oss << "expected next token to be " << t << ", got " << peekToken.Type << " instead";
    errors.push_back(oss.str());
}

void Parser::registerPrefix(TokenType type, prefixParseFn fn) {
    prefixParseFns[type] = fn;
}

void Parser::registerInfix(TokenType type, infixParseFn fn) {
    infixParseFns[type] = fn;
}

#TODO ...other methods
