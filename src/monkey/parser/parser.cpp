#include "parser.hpp"

std::unordered_map<TokenType, Precedence> precedences = {
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
    // Initialize errors
    errors = std::vector<std::string>();


    // Register prefix functions using lambda functions for explicit casting
    registerPrefix(TokenType::IDENT, [this]() -> std::unique_ptr<Expression> {
        return this->parseIdentifier();
    });
    registerPrefix(TokenType::INT, [this]() -> std::unique_ptr<Expression> {
        return this->parseIntegerLiteral();
    });
    registerPrefix(TokenType::BANG, [this]() -> std::unique_ptr<Expression> {
        return this->parsePrefixExpression();
    });
    registerPrefix(TokenType::MINUS, [this]() -> std::unique_ptr<Expression> {
        return this->parsePrefixExpression();
    });
    registerPrefix(TokenType::TRUE, [this]() -> std::unique_ptr<Expression> {
        return this->parseBoolean();
    });
    registerPrefix(TokenType::FALSE, [this]() -> std::unique_ptr<Expression> {
        return this->parseBoolean();
    });
    registerPrefix(TokenType::LPAREN, [this]() -> std::unique_ptr<Expression> {
        return this->parseGroupedExpression();
    });
    registerPrefix(TokenType::IF, [this]() -> std::unique_ptr<Expression> {
        return this->parseIfExpression();
    });
    registerPrefix(TokenType::FUNCTION, [this]() -> std::unique_ptr<Expression> {
        return this->parseFunctionLiteral();
    });

    // Register infix functions using lambda functions for explicit casting
    registerInfix(TokenType::PLUS, [this](std::unique_ptr<Expression> left) -> std::unique_ptr<Expression> {
        return this->parseInfixExpression(std::move(left));
    });
    registerInfix(TokenType::MINUS, [this](std::unique_ptr<Expression> left) -> std::unique_ptr<Expression> {
        return this->parseInfixExpression(std::move(left));
    });
    registerInfix(TokenType::SLASH, [this](std::unique_ptr<Expression> left) {
        return this->parseInfixExpression(std::move(left));
    });
    registerInfix(TokenType::ASTERISK, [this](std::unique_ptr<Expression> left) {
        return this->parseInfixExpression(std::move(left));
    });
    registerInfix(TokenType::EQ, [this](std::unique_ptr<Expression> left) {
        return this->parseInfixExpression(std::move(left));
    });
    registerInfix(TokenType::NOT_EQ, [this](std::unique_ptr<Expression> left) {
        return this->parseInfixExpression(std::move(left));
    });
    registerInfix(TokenType::LT, [this](std::unique_ptr<Expression> left) {
        return this->parseInfixExpression(std::move(left));
    });
    registerInfix(TokenType::GT, [this](std::unique_ptr<Expression> left) {
        return this->parseInfixExpression(std::move(left));
    });
    registerInfix(TokenType::LPAREN, [this](std::unique_ptr<Expression> function) {
        return this->parseCallExpression(std::move(function));
    });

    // Read two tokens, so curToken and peekToken are both set
    nextToken();
    nextToken();
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
    return Precedence::LOWEST;
}

int Parser::curPrecedence() const {
    auto it = precedences.find(curToken.Type);
    if (it != precedences.end()) {
        return it->second;
    }
    return Precedence::LOWEST;
}

// Parsing functions here...

std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();
    program->Statements = std::vector<std::unique_ptr<Statement>>();

    while(!curTokenIs(TokenType::EOF_TOKEN)) {
        auto stmt = parseStatement();
        if(stmt) program->Statements.push_back(std::move(stmt));
        nextToken();
    }

    return program;
}

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

std::unique_ptr<Expression> Parser::parseExpression(Precedence pVal){
    auto prefixIt = prefixParseFns.find(curToken.Type);
    if (prefixIt == prefixParseFns.end()) {
        noPrefixParseFnError(curToken.Type);
        return nullptr;
    }
    auto prefix = prefixIt->second;
    std::unique_ptr<Expression> leftExp = (prefix)();
    //here we are calling prefix like a function and the parentheses after prefix 
    //are invoking the callable object. if prefix is a lambda or std::function
    //wrapping a lambda it will invoke the lambdas code. 
    //The result is stored in leftExp

    while(!peekTokenIs(TokenType::SEMICOLON) && pVal < peekPrecedence()) {
        auto infixIt = infixParseFns.find(peekToken.Type);
        if (infixIt == infixParseFns.end()) return leftExp;
        nextToken();

        auto infix = infixIt->second;
        leftExp = (infix)(std::move(leftExp));
        //std::move is used to transfer the ownership of the std::unique_ptr to the function
    }

    return leftExp;

}

std::unique_ptr<Identifier>  Parser::parseIdentifier(){
    return std::make_unique<Identifier> (curToken, curToken.Literal);
}

std::unique_ptr<IntegerLiteral>  Parser::parseIntegerLiteral(){
    auto lit = std::make_unique<IntegerLiteral>();
    lit->token = curToken;

    try {
        //stoi can throw an exception if conversion fails
        lit->Value = std::stoll(curToken.Literal);
    } catch (const std::exception& e){
        std::string msg = "could not parse \"" + curToken.Literal + "\" as integer";
        errors.push_back(msg);
        return nullptr;
    }

    return lit;
}

std::unique_ptr<PrefixExpression>  Parser::parsePrefixExpression(){
    auto expression = std::make_unique<PrefixExpression>(curToken, curToken.Literal);

    nextToken();

    expression->Right = parseExpression(Precedence::PREFIX);

    return expression;

}

std::unique_ptr<InfixExpression> Parser::parseInfixExpression (std::unique_ptr<Expression> left){
    auto expression = std::make_unique<InfixExpression>(curToken, curToken.Literal, std::move(left));
    auto precedence = curPrecedence();

    nextToken();
    expression->Right = parseExpression(static_cast<Precedence>(precedence));

    return expression;
}

std::unique_ptr<Boolean> Parser::parseBoolean(){
    return std::make_unique<Boolean> (curToken, curTokenIs(TokenType::TRUE));
}

std::unique_ptr<Expression> Parser::parseGroupedExpression(){
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

    nextToken();

    while (!curTokenIs(TokenType::RBRACE) && !curTokenIs(TokenType::EOF_TOKEN)) {
        auto stmt = parseStatement();
        if (stmt) {
            block->Statements.push_back(std::move(stmt));
        }
        nextToken();
    }

    return block;
}

std::unique_ptr<FunctionLiteral> Parser::parseFunctionLiteral(){
    auto lit = std::make_unique<FunctionLiteral>(curToken);

    if(!expectPeek(TokenType::LPAREN)) {
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
    std::vector<std::unique_ptr<Identifier>> identifiers;

    if(peekTokenIs(TokenType::RPAREN)) {
        nextToken();
        return identifiers;
    }

    nextToken();

    auto ident = std::make_unique<Identifier>(curToken, curToken.Literal);
    identifiers.push_back(std::move(ident));

    while (peekTokenIs(TokenType::COMMA)){
        nextToken();  // Consume the COMMA
        nextToken();  // Move to the next token after the COMMA
        ident = std::make_unique<Identifier>(curToken, curToken.Literal);
        identifiers.push_back(std::move(ident));
    }

    if (!expectPeek(TokenType::RPAREN)) {
        // in C++ we can't return nullptr for a vector.
        // For now, I'll clear the vector and return it, indicating an error.
        identifiers.clear();
        return identifiers;
    }

    return identifiers;


}

std::unique_ptr<CallExpression> Parser::parseCallExpression(std::unique_ptr<Expression> function){
    auto exp = std::make_unique<CallExpression>(curToken, function);
    exp->Arguments = parseCallArguments();
    return exp;
}

std::vector<std::unique_ptr<Expression>> Parser::parseCallArguments(){
    std::vector<std::unique_ptr<Expression>> args;

    if(peekTokenIs(TokenType::RPAREN)) {
        nextToken();
        return args;
    }

    nextToken();
    args.push_back(parseExpression(Precedence::LOWEST));

    while(peekTokenIs(TokenType::COMMA)){  //todo check this
        nextToken(); // consume the COMMA
        nextToken(); // move to the next token after the COMMA
        args.push_back(parseExpression(Precedence::LOWEST));
    }

    if(!expectPeek(TokenType::RPAREN)){
        // Return an empty vector to indicate an error or unexpected input
        args.clear();
        return args;
    }

    return args;
}

std::vector<std::string> Parser::Errors() const {
    return errors; 
}
