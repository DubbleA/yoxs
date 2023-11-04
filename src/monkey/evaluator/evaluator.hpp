// evaluator.hpp
#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "../ast/ast.hpp"
#include "../object/object.hpp"
#include "../object/environment.hpp"
#include <memory>

class Evaluator {
    friend struct EvaluatorVisitor;  // Grant EvaluatorVisitor access to private members
public:
    using NodeVariant = std::variant<
    //Statements
    std::shared_ptr<Program>,
    std::shared_ptr<BlockStatement>,
    std::shared_ptr<ExpressionStatement>,
    std::shared_ptr<ReturnStatement>,
    std::shared_ptr<LetStatement>,
    //Expressions
    std::shared_ptr<IntegerLiteral>,
    std::shared_ptr<Boolean>,
    std::shared_ptr<Expression>,
    std::shared_ptr<PrefixExpression>,
    std::shared_ptr<InfixExpression>,
    std::shared_ptr<IfExpression>,
    std::shared_ptr<Identifier>,
    std::shared_ptr<FunctionLiteral>,
    std::shared_ptr<CallExpression>
    >;
    
    static std::shared_ptr<Object> Eval(NodeVariant node, std::shared_ptr<Environment> env);

private:
    static std::shared_ptr<Object> evalProgram(std::shared_ptr<Program> program, std::shared_ptr<Environment> env);
    static std::shared_ptr<Object> evalBlockStatement(std::shared_ptr<BlockStatement> block, std::shared_ptr<Environment> env);
    // ... Other evaluation functions for different node types

    static bool isTruthy(std::shared_ptr<Object> obj);
    static bool isError(std::shared_ptr<Object> obj);
    // ... Other helper functions
};

struct EvaluatorVisitor {
    std::shared_ptr<Environment> env;

    std::shared_ptr<Object> operator()(std::shared_ptr<Program> node){
        return Evaluator::evalProgram(node, env);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<BlockStatement> node){
        return Evaluator::evalBlockStatement(node, env);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<ExpressionStatement> node) {
        // Since node->expr is a std::unique_ptr<Expression>, convert it to std::shared_ptr<Node>
        // before passing it to Eval.
        return Evaluator::Eval(Evaluator::NodeVariant{std::move(node->expr)}, env);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<ReturnStatement> node) {
        auto val = Evaluator::Eval(Evaluator::NodeVariant{std::move(node->ReturnValue)}, env);
        if(Evaluator::isError(val)) {
            return val;
        }
        return std::make_shared<ReturnValue>(val);  // Assuming ReturnValue is a suitable type
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<LetStatement> node) {
        auto val = Evaluator::Eval(Evaluator::NodeVariant{std::move(node->Value)}, env);
        if(Evaluator::isError(val)) {
            return val;
        }
        env->Set(node->Name->Value(), val);
        return nullptr;
    }
    //expressions
    std::shared_ptr<Object> operator()(std::shared_ptr<IntegerLiteral> node){
        return std::make_shared<Integer>(node->Value);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<Boolean> node){
        return nativeBoolToBooleanObject(node->Value);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<PrefixExpression> node){
        auto right = Evaluator::Eval(Evaluator::NodeVariant{std::move(node->Right)}, env);
        if(Evaluator::isError(right)) {
            return right;
        }
        return evalPrefixExpression(node->Operator, right);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<InfixExpression> node){
        auto left = Evaluator::Eval(Evaluator::NodeVariant{std::move(node->Left)}, env);
        if(Evaluator::isError(left)){
            return left;
        }

        auto right = Evaluator::Eval(Evaluator::NodeVariant{std::move(node->Right)}, env);
        if(Evaluator::isError(right)) {
            return right;
        }
        return evalInfixExpression(node->Operator, left, right);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<IfExpression> node){
        return evalIfExpression(std::move(node), env);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<Identifier> node){
        return evalIdentifier(std::move(node), env);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<FunctionLiteral> node){
        auto params = std::move(node->Parameters);
        auto body = std::move(node->Body);
        return std::make_shared<Function>(params, env, body);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<CallExpression> node){
        auto function = Evaluator::Eval(Evaluator::NodeVariant{std::move(node->Function)}, env);
        
        if(Evaluator::isError(function)){
            return function;
        }

        auto args = evalExpressions(node->Arguments, env);
        if(args.size() == 1 && Evaluator::isError(args[0])){
            return args[0];
        }
        return applyFunction(function, args);
    }

};

class ObjectConstants {
public:
    static std::shared_ptr<Null> NULL_OBJ;
    static std::shared_ptr<Boolean> TRUE;
    static std::shared_ptr<Boolean> FALSE;
};

#endif // EVALUATOR_H
