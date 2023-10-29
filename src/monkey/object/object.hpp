// object.hpp
#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include "../ast/ast.hpp"
#include "environment.hpp"

enum ObjectType {
    NULL_OBJ,
    ERROR_OBJ,
    INTEGER_OBJ,
    BOOLEAN_OBJ,
    RETURN_VALUE_OBJ,
    FUNCTION_OBJ
};

class Object {
public:
    virtual ObjectType Type() = 0;
    virtual std::string Inspect() const = 0;
};

class Integer : public Object {
public:
    int64_t Value;

    Integer(int64_t value) : Value(value) {}
    ObjectType Type() override { return INTEGER_OBJ; }
    std::string Inspect() const override { return std::to_string(Value); }
};

class Boolean : public Object {
public:
    bool Value;

    Boolean(bool value) : Value(value) {}
    ObjectType Type() override { return BOOLEAN_OBJ; }
    std::string Inspect() const override { return Value ? "true" : "false"; }
};

class Null : public Object {
public:
    ObjectType Type() override { return NULL_OBJ; }
    std::string Inspect() const override { return "null"; }
};

class ReturnValue : public Object {
public:
    std::unique_ptr<Object> Value;

    ReturnValue(std::unique_ptr<Object> value) : Value(std::move(value)) {}
    ObjectType Type() override { return RETURN_VALUE_OBJ; }
    std::string Inspect() const override { return Value->Inspect(); }
};

class Error : public Object {
public:
    std::string Message;

    Error(const std::string& message) : Message(message) {}
    ObjectType Type() override { return ERROR_OBJ; }
    std::string Inspect() const override { return "ERROR: " + Message; }
};

class Function : public Object {
public:
    std::vector<std::unique_ptr<Identifier>> Parameters;
    std::unique_ptr<BlockStatement> Body;
    std::unique_ptr<Environment> Env;

    Function(const std::vector<std::unique_ptr<Identifier>>& parameters, std::unique_ptr<BlockStatement> body, std::unique_ptr<Environment> env)
        : Parameters(parameters), Body(std::move(body)), Env(std::move(env)) {}
    ObjectType Type() override { return FUNCTION_OBJ; }
    std::string Inspect() const override;
};

#endif // OBJECT_H