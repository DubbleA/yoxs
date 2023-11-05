// evaluator.hpp
#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "../ast/ast.hpp"
#include "../object/object.hpp"
#include "../object/environment.hpp"
#include <memory>

using namespace YOXS_OBJECT;
struct EvaluatorVisitor;

class Evaluator {
    friend struct EvaluatorVisitor;  // Grant EvaluatorVisitor access to private members
public:
    using NodeVariant = std::variant<
    //Statements
    std::shared_ptr<Statement>, //extra
    std::shared_ptr<Program>,
    std::shared_ptr<BlockStatement>,
    std::shared_ptr<ExpressionStatement>,
    std::shared_ptr<ReturnStatement>,
    std::shared_ptr<LetStatement>,
    //Expressions
    std::shared_ptr<IntegerLiteral>,
    std::shared_ptr<YOXS_OBJECT::Boolean>,
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
    static std::shared_ptr<YOXS_OBJECT::Boolean> nativeBoolToBooleanObject(bool input);
    static std::shared_ptr<Object> evalPrefixExpression(const std::string& op, std::shared_ptr<Object> right);
    static std::shared_ptr<Object> evalInfixExpression(const std::string& op, std::shared_ptr<Object> left, std::shared_ptr<Object> right);
    static std::shared_ptr<Object> evalBangOperatorExpression(std::shared_ptr<Object> right);
    static std::shared_ptr<Object> evalMinusPrefixOperatorExpression(std::shared_ptr<Object> right);
    static std::shared_ptr<Object> evalIntegerInfixExpression(const std::string& op, std::shared_ptr<Object> left, std::shared_ptr<Object> right);
    static std::shared_ptr<Object> evalIfExpression(std::shared_ptr<IfExpression> ie, std::shared_ptr<Environment> env);
    static std::shared_ptr<Object> evalIdentifier(std::shared_ptr<Identifier> node, std::shared_ptr<Environment> env);
    
    static bool isTruthy(std::shared_ptr<Object> obj);
    static std::shared_ptr<Error> newError(const std::string format, ...);
    static bool isError(std::shared_ptr<Object> obj);
    static std::vector<std::shared_ptr<Object>> evalExpressions(std::vector<std::shared_ptr<Expression>> exps, std::shared_ptr<Environment> env);
    static std::shared_ptr<Object> applyFunction(std::shared_ptr<Object> fn, std::vector<std::shared_ptr<Object>> args);
    static std::shared_ptr<Environment> extendFunctionEnv(std::shared_ptr<Function> fn, std::vector<std::shared_ptr<Object>> args);
    static std::shared_ptr<Object> unwrapReturnValue(std::shared_ptr<Object> obj);
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

    std::shared_ptr<Object> operator()(std::shared_ptr<YOXS_OBJECT::Boolean> node){
        return Evaluator::nativeBoolToBooleanObject(node->Value);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<PrefixExpression> node){
        auto right = Evaluator::Eval(Evaluator::NodeVariant{std::move(node->Right)}, env);
        if(Evaluator::isError(right)) {
            return right;
        }
        return Evaluator::evalPrefixExpression(node->Operator, right);
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
        return Evaluator::evalInfixExpression(node->Operator, left, right);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<IfExpression> node){
        return Evaluator::evalIfExpression(std::move(node), env);
    }

    std::shared_ptr<Object> operator()(std::shared_ptr<Identifier> node){
        return Evaluator::evalIdentifier(std::move(node), env);
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

        auto args = Evaluator::evalExpressions(node->Arguments, env);
        if(args.size() == 1 && Evaluator::isError(args[0])){
            return args[0];
        }
        return Evaluator::applyFunction(function, args);
    }

    // std::shared_ptr<Object> operator()(std::shared_ptr<Statement> node){
    //     return nullptr;
    // }

    // Fallback for other types
    template <typename T>
    std::shared_ptr<Object> operator()(const T&) const {
        return nullptr;
    }
};

class ObjectConstants {
public:
    static std::shared_ptr<Null> NULL_OBJ;
    static std::shared_ptr<YOXS_OBJECT::Boolean> TRUE;
    static std::shared_ptr<YOXS_OBJECT::Boolean> FALSE;
};

#endif // EVALUATOR_H
