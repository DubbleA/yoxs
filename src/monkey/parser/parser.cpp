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
    
    std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
        auto stmt = std::make_unique<ReturnStatement>();
        stmt->token = curToken;

        nextToken();
        stmt->ReturnValue = parseExpression(Precedence::LOWEST);

        if (peekTokenIs(TokenType::SEMICOLON)) {
            nextToken();
        }

        return stmt;
    }

    std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement(){
        auto stmt = std::make_unique<ExpressionStatement>();
        stmt->expr = parseExpression(Precedence::LOWEST); // check if valid

        if (peekTokenIs(TokenType::SEMICOLON)){
            nextToken();
        }

        return stmt;
    }

    std::unique_ptr<Expression>  Parser::parseExpression(Precedence pVal){
        auto prefix = prefixParseFns[curToken.Type];
        if(prefix == nullptr){
            noPrefixParseFnError(curToken.Type);
            return nullptr;
        }
        auto leftExp = prefix();

        for(!peekTokenIs(TokenType::SEMICOLON) && pVal < peekPrecedence()){
            //do this shit
        }

        return leftExp;

    }

    std::unique_ptr<Identifier>  Parser::parseIdentifier(){
        return std::make_unique<Identifier> (curToken, curToken.Literal);
    }

    std::unique_ptr<IntegerLiteral>  Parser::parseIntegerLiteral(){
        auto lit = std::make_unique<IntegerLiteral> (curToken);

        //parse stuff
    }
    
    std::unique_ptr<PrefixExpression>  Parser::parsePrefixExpression(){
        auto expression = std::make_unique<PrefixExpression>(curToken, curToken.Literal);

        nextToken();

        expression->Right = parseExpression(Precedence::PREFIX);

        return expression;

    }

    std::unique_ptr<InfixExpression>  Parser::parseInfixExpression(Expression* left){
        auto expression = std::make_unique<InfixExpression>(curToken, curToken.Literal, left);
        auto precedence = curPrecedence();

        nextToken();
        expression->Right = parseExpression(precedence);

        return expression;
    }

    std::unique_ptr<Boolean>  Parser::parseBoolean(){
        return std::make_unique<Boolean> (curToken, curTokenIs(TokenType::TRUE));
    }

    std::unique_ptr<Expression>  Parser::parseGroupedExpression(){
        nextToken();

        auto exp = parseExpression(Precedence::LOWEST);

        if(!expectPeek(TokenType::RPAREN)) {
            return nullptr;
        }

        return exp;
    }

    std::unique_ptr<IfExpression>  Parser::parseIfExpression() {
        auto expression = std::make_unique<IfExpression>(curToken);

        if(!expectPeek(TokenType::LPAREN)) {
            return nullptr;
        }

        nextToken();
        expression->Condition = parseExpression(Precedence::LOWEST);

        if(!expectPeek(TokenType::RPAREN)) {
            return nullptr;
        }

        if(!expectPeek(TokenType::LBRACE)){
            return nullptr;
        }

        expression->Consequence = parseBlockStatement();

        if(peekTokenIs(TokenType::ELSE)) {
            nextToken();

            if(!expectPeek(TokenType::LBRACE)) {
                return nullptr;
            }

            expression->Alternative = parseBlockStatement();
        }

        return expression;
    }

    std::unique_ptr<BlockStatement> Parser::parseBlockStatement(){
        auto block = std::make_unique<BlockStatement>(curToken);
        block->Statements = new std::vector<std::unique_ptr<Statement>>; //todo

        nextToken();

        for(){
            //todo
        }
        return block;
    }

    std::unique_ptr<FunctionLiteral> Parser::parseFunctionLiteral(){
        auto lit = std::make_unique<FunctionLiteral>(curToken);

        if(expectPeek(TokenType::LPAREN)) {
            return nullptr;
        }

        lit->Parameters = parseFunctionParameters();

        if(!expectPeek(TokenType::LBRACE)) {
            return nullptr;
        }

        lit->Body = parseBlockStatement();

        return lit;
    }
    std::vector<std::unique_ptr<Identifier>>  Parser::parseFunctionParameters() {
        auto identifiers = std::make_unique<Identifier>(); // wrong

        if(peekTokenIs(TokenType::RPAREN)) {
            nextToken();
            return identifiers;
        }

        nextToken();

        auto ident = std::make_unique<Identifier>(curToken, curToken.Literal);

    }
    std::unique_ptr<CallExpression> Parser::parseCallExpression(Expression* function){
        auto exp = std::make_unique<CallExpression>(curToken, function);
        exp->Arguments = parseCallArguments();
        return exp;
    }
    std::vector<std::unique_ptr<Expression>> Parser::parseCallArguments(){
        std::vector<std::unique_ptr<Expression>> args; //could be wrong

        if(peekTokenIs(TokenType::RPAREN)) {
            nextToken();
            return args;
        }

        while(peekTokenIs(TokenType::COMMA)){  //todo check this
            nextToken();
            nextToken();
            //append todo
        }

        if(!expectPeek(TokenType::RPAREN)){
            return; // check if void null return is okay
        }

        return args;
    }



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
