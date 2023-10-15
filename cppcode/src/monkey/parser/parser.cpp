#include "parser.hpp"

std::unordered_map<TokenType, Precedence> precedences = {
    { EQ, EQUALS },
    { NOT_EQ, EQUALS },
    #TODO ...other precedences
};

Parser::Parser(Lexer& l) : lexer(&l) {
    #TODO Initialization code here...
}

Parser::~Parser() {
    #TODO Cleanup code here...
}

std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();
    #TODO Parsing code here...
    return program;
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = lexer->NextToken();
}

#TODO ...other methods
