#include "parser.hpp"

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

    // Register prefix and infix functions...
    // Note: You need to translate each parse function from Go and then register here.
    // E.g., registerPrefix(TokenType::IDENT, &Parser::parseIdentifier);
    // TODO: Other initializations
}

void Parser::registerPrefix(TokenType type, prefixParseFn fn) {
    prefixParseFns[type] = fn;
}

void Parser::registerInfix(TokenType type, infixParseFn fn) {
    infixParseFns[type] = fn;
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

void Parser::noPrefixParseFnError(TokenType t) {
    std::ostringstream oss;
    oss << "no prefix parse function for " << t << " found";
    errors.push_back(oss.str());
}

int Parser::peekPrecedence() const {
    auto it = precedences.find(peekToken.Type);
    if (it != precedences.end()) {
        return it->second;
    }
    return LOWEST;
}

int Parser::curPrecedence() const {
    auto it = precedences.find(curToken.Type);
    if (it != precedences.end()) {
        return it->second;
    }
    return LOWEST;
}

// Parsing functions here...

    std::unique_ptr<Statement> Parser::parseStatement(){
        switch (curToken.Type)
        {
        case TokenType::LET:
            return parseLetStatement();
            break;
        
        case TokenType::RETURN:
            return parseReturnStatement();
            break;
        
        default: //expression statement
            return parseExpressionStatement();
            break;
        }
    }

    std::unique_ptr<LetStatement> Parser::parseLetStatement() {
        auto stmt = std::make_unique<LetStatement>();
        stmt->token = curToken;

        if (!expectPeek(TokenType::IDENT)) {
            return nullptr;
        }

        stmt->Name = std::make_unique<Identifier> (curToken, curToken.Literal); 

        if(!expectPeek(TokenType::ASSIGN)) {
            return nullptr;
        }

        nextToken();
        stmt->Value = parseExpression(Precedence::LOWEST);

        if(peekTokenIs(TokenType::SEMICOLON)){
            nextToken();
        }

        return stmt;
    }
    
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();

    std::unique_ptr<Expression> parseExpression(Precedence pVal);

    std::unique_ptr<Identifier> parseIdentifier();
    std::unique_ptr<IntegerLiteral> parseIntegerLiteral();

    std::unique_ptr<PrefixExpression> parsePrefixExpression();
    std::unique_ptr<InfixExpression> parseInfixExpression(Expression* left);
    std::unique_ptr<Boolean> parseBoolean();
    std::unique_ptr<Expression> parseGroupedExpression();
    std::unique_ptr<IfExpression> parseIfExpression();
    std::unique_ptr<BlockStatement> parseBlockStatement();
    std::unique_ptr<FunctionLiteral> parseFunctionLiteral();
    std::vector<std::unique_ptr<Identifier>> parseFunctionParameters();     std::unique_ptr<CallExpression> parseCallExpression(Expression* function);
    std::vector<std::unique_ptr<Expression>> parseCallArguments();



Parser::~Parser() {
    // Any cleanup if required...
}

std::vector<std::string> Parser::Errors() const {
    return errors; 
}

std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();
    while(!curTokenIs(TokenType::EOF_TOKEN)) {
        //todo
    }
    return program;
}
