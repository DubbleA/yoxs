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

Identifier::Identifier(const Token& t, const std::string& v) : token(t) {
    token.Literal = v;
}

InfixExpression::InfixExpression(const Token& tok, const std::string& op, std::unique_ptr<Expression> leftExp) : token(tok), Operator(op), Left(std::move(leftExp)) {}

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

std::string IntegerLiteral::TokenLiteral() const {
    return token.Literal;
}

std::string IntegerLiteral::String() const {
    return token.Literal;
}

std::string PrefixExpression::TokenLiteral() const {
    return token.Literal;
}

std::string PrefixExpression::String() const {
    return "(" + Operator + Right->String() + ")";
}

std::string InfixExpression::TokenLiteral() const {
    return token.Literal;
}

std::string InfixExpression::String() const {
    return "(" + Left->String() + " " + Operator + " " + Right->String() + ")";
}

std::string IfExpression::TokenLiteral() const {
    return token.Literal;
}

std::string IfExpression::String() const {
    std::string result = "if" + Condition->String() + " " + Consequence->String();
    if (Alternative) {
        result += "else " + Alternative->String();
    }
    return result;
}

std::string FunctionLiteral::TokenLiteral() const {
    return token.Literal;
}

std::string FunctionLiteral::String() const {
    std::string result = token.Literal + "(";
    std::vector<std::string> params;
    for (const auto& param : Parameters) {
        params.push_back(param->String());
    }
    result += join(params, ", ") + ") " + Body->String();
    return result;
}

std::string CallExpression::TokenLiteral() const {
    return token.Literal;
}

std::string CallExpression::String() const {
    std::string result = Function->String() + "(";
    std::vector<std::string> args;
    for (const auto& arg : Arguments) {
        args.push_back(arg->String());
    }
    result += join(args, ", ") + ")";
    return result;
}

std::string join(const std::vector<std::string>& elements, const std::string& delimiter) {
    switch (elements.size()) {
        case 0:
            return "";
        case 1:
            return elements[0];
        default:
            std::ostringstream os;
            std::copy(elements.begin(), elements.end() - 1, std::ostream_iterator<std::string>(os, delimiter.c_str()));
            os << *elements.rbegin();
            return os.str();
    }
}