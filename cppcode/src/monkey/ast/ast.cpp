#include "ast.hpp"

std::string Program::TokenLiteral() const {
        if(!Statements.empty()){
            return Statements[0]->TokenLiteral();
        }
        else{
            return "";
        }
    }


std::string Program::String() const {
    std::stringstream out;
    for(const auto& stmt : Statements){
        out << stmt->String();
    }
    return out.str();
}

std::string LetStatement::TokenLiteral() const {
    return token.Literal;
}

std::string LetStatement::String() const {
    return token.Literal + " " + Name->String() + " = " + (Value ? Value->String() : "") + ";";
}

std::string ReturnStatement::TokenLiteral() const {
    return token.Literal;
}

std::string ReturnStatement::String() const {
    std::stringstream out;
    out << TokenLiteral() << " ";
    if(ReturnValue) {
        out << ReturnValue->String();
    }
    out << ";";
    return out.str();
}

std::string ExpressionStatement::TokenLiteral() const {
    return token.Literal;
}

std::string ExpressionStatement::String() const {
    return (expr ? expr->String() : "");
}

std::string BlockStatement::TokenLiteral() const {
    return token.Literal;
}

std::string BlockStatement::String() const {
    std::stringstream out;
    for (const auto& stmt : Statements) {
        out << stmt->String();
    }
    return out.str();
}

std::string Identifier::Value() const {
    return token.Literal;
}

std::string Identifier::TokenLiteral() const {
    return token.Literal;
}

std::string Identifier::String() const {
    return token.Literal;
}

std::string Boolean::TokenLiteral() const {
    return token.Literal;
}

std::string Boolean::String() const {
    return token.Literal;
}