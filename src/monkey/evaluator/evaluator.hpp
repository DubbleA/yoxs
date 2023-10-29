// evaluator.hpp
#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "../ast/ast.hpp"
#include "../object/object.hpp"
#include "../object/environment.hpp"
#include <memory>

class Evaluator {
public:
    static std::shared_ptr<Object> Eval(std::shared_ptr<Node> node, std::shared_ptr<Environment> env);

private:
    static std::shared_ptr<Object> evalProgram(std::shared_ptr<Program> program, std::shared_ptr<Environment> env);
    static std::shared_ptr<Object> evalBlockStatement(std::shared_ptr<BlockStatement> block, std::shared_ptr<Environment> env);
    // ... Other evaluation functions for different node types

    static bool isTruthy(std::shared_ptr<Object> obj);
    static bool isError(std::shared_ptr<Object> obj);
    // ... Other helper functions
};

class ObjectConstants {
public:
    static std::shared_ptr<Null> NULL_OBJ;
    static std::shared_ptr<Boolean> TRUE;
    static std::shared_ptr<Boolean> FALSE;
};

#endif // EVALUATOR_H
