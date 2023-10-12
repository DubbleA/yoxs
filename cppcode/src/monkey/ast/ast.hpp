#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <sstream>
#include "../token/token.hpp"

// Forward declarations of all the classes we're going to use.
class Statement;
class Expression;

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
private:
    std::vector<std::unique_ptr<Statement>> Statements;
public:
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
    Token token; // the '{' token
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
    Token token; // The IDENT token
    std::string Value() const;

    std::string TokenLiteral() const override;

    std::string String() const override;

};

class Boolean : public Expression {
public:
    Token token;
    bool Value;

    std::string TokenLiteral() const override;
    std::string String() const override;
};


#endif //AST_H