#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <iterator>
#include "../token/token.hpp"

// Forward declarations of all the classes we're going to use.
class Statement;
class Expression;
class Identifier;

// Node represents every node in the abstract syntax tree
class Node {
public:
    virtual ~Node() = default; 
    virtual std::string TokenLiteral() const = 0;
    virtual std::string String() const = 0;
};

// All nodes that can be used as statements implement this interface
class Statement : public Node {
public: 
    virtual void statementNode() = 0;
};

// All nodes that can be used as expressions implement this interface
class Expression : public Node {
public: 
    virtual void expressionNode() = 0;
};

// The root node of every AST our parser produces
class Program : public Node {
public:
    std::vector<std::unique_ptr<Statement>> Statements;
    std::string TokenLiteral() const override;
    std::string String() const override;
};

// AST node for let statements
class LetStatement : public Statement {
public:
    Token token; // The 'let' token
    std::unique_ptr<Identifier> Name;
    std::unique_ptr<Expression> Value;

    std::string TokenLiteral() const override;
    std::string String() const override;
    void statementNode() override {}

};

class ReturnStatement : public Statement {
public:
    Token token; // the 'return' token
    std::unique_ptr<Expression> ReturnValue;

    std::string TokenLiteral() const override;
    std::string String() const override;
};

class ExpressionStatement : public Statement {
public:
    Token token; // the first token of the expression
    std::unique_ptr<Expression> expr;

    std::string TokenLiteral() const override;
    std::string String() const override;
};

class BlockStatement : public Statement {
public:
    Token token; // the '{' token
    std::vector<std::unique_ptr<Statement>> Statements;

    std::string TokenLiteral() const override;
    std::string String() const override;
};

class Identifier : public Expression {
public:
    Identifier(const Token& t, const std::string& v);

    Token token; // The IDENT token
    std::string Value() const;

    std::string TokenLiteral() const override;

    std::string String() const override;
    void expressionNode() override {}

};

class Boolean : public Expression {
public:
    Token token;
    bool Value;

    std::string TokenLiteral() const override;
    std::string String() const override;
};

class IntegerLiteral : public Expression {
public: 
    Token token;
    int64_t Value;

    std::string TokenLiteral() const override;
    std::string String() const override;
};

class PrefixExpression : public Expression {
public:
    Token token; // The prefix token, e.g. !
    std::string Operator;
    std::unique_ptr<Expression> Right;

    std::string TokenLiteral() const override;
    std::string String() const override;
};

class InfixExpression : public Expression {
public:
    Token token; // The operator token, e.g. +
    std::unique_ptr<Expression> Left;
    std::string Operator;
    std::unique_ptr<Expression> Right;

    std::string TokenLiteral() const override;
    std::string String() const override;
};

class IfExpression : public Expression {
public:
    Token token; // The 'if' token
    std::unique_ptr<Expression> Condition;
    std::unique_ptr<BlockStatement> Consequence;
    std::unique_ptr<BlockStatement> Alternative;

    std::string TokenLiteral() const override;
    std::string String() const override;
};

class FunctionLiteral : public Expression {
public:
    Token token; // The 'fn' token
    std::vector<std::unique_ptr<Identifier>> Parameters;
    std::unique_ptr<BlockStatement> Body;

    std::string TokenLiteral() const override;
    std::string String() const override;
};

class CallExpression : public Expression {
public:
    Token token; // The '(' token
    std::unique_ptr<Expression> Function; // Identifier or FunctionLiteral
    std::vector<std::unique_ptr<Expression>> Arguments;

    std::string TokenLiteral() const override;
    std::string String() const override;
};

std::string join(const std::vector<std::string>&, const std::string&);



#endif //AST_H