// object.hpp
#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <memory>
#include <vector>
#include "../ast/ast.hpp"

namespace YOXS_OBJECT {

class Environment;  // Forward declaration
class Object;

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

class BooleanObject : public Object {
public:
    bool Value;

    BooleanObject(bool value) : Value(value) {}
    ObjectType Type() override { return BOOLEAN_OBJ; }
    std::string Inspect() const override { return Value ? "true" : "false"; }
};

class NullObject : public Object {
public:
    ObjectType Type() override { return NULL_OBJ; }
    std::string Inspect() const override { return "null"; }
};

class ReturnValue : public Object {
public:
    std::shared_ptr<Object> Value;

    ReturnValue(std::shared_ptr<Object> value) : Value(std::move(value)) {}
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
    std::vector<std::shared_ptr<YOXS_AST::Identifier>> Parameters;
    std::shared_ptr<YOXS_AST::BlockStatement> Body;
    std::shared_ptr<Environment> Env;

    Function(const std::vector<std::shared_ptr<YOXS_AST::Identifier>>& parameters, std::shared_ptr<Environment> env, std::shared_ptr<YOXS_AST::BlockStatement> body)
        : Parameters(parameters), Env(std::move(env)), Body(std::move(body)) {}
    ObjectType Type() override { return FUNCTION_OBJ; }
    std::string Inspect() const override;
};

std::string ObjectTypeToString(ObjectType type);


} //namespace of YOXS_OBJECT

#include "environment.hpp" //have to declare it down here cuz it mess shit up when both headers rely on each other

#endif // OBJECT_H