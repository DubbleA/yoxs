#include "evaluator.hpp"

//evaluator.cpp

std::shared_ptr<NullObject> ObjectConstants::NULL_OBJ = std::make_shared<NullObject>();
std::shared_ptr<BooleanObject> ObjectConstants::TRUE = std::make_shared<BooleanObject>(true);
std::shared_ptr<BooleanObject> ObjectConstants::FALSE = std::make_shared<BooleanObject>(false);

std::shared_ptr<Object> Evaluator::Eval(std::shared_ptr<Node> node, std::shared_ptr<Environment> env) {
    // The dynamic_cast will check the actual type of Node and return nullptr if the cast is not valid.
    if (auto n = std::dynamic_pointer_cast<Program>(node)) {
        return evalProgram(n, env);
    } else if (auto n = std::dynamic_pointer_cast<BlockStatement>(node)) {
        return evalBlockStatement(n, env);
    } else if (auto n = std::dynamic_pointer_cast<ExpressionStatement>(node)) {
        return Eval(n->expr, env);
    } else if (auto n = std::dynamic_pointer_cast<ReturnStatement>(node)) {
        auto val = Eval(n->ReturnValue, env);
        if (isError(val)) {
            return val;
        }
        return std::make_shared<ReturnValue>(val);
    } else if (auto n = std::dynamic_pointer_cast<LetStatement>(node)){
        auto val = Eval(n->Value, env);
        if(Evaluator::isError(val)) {
            return val;
        }
        env->Set(n->Name->Value(), val);
    } else if (auto n = std::dynamic_pointer_cast<IntegerLiteral>(node)){
        return std::make_shared<Integer>(n->Value);
    } else if (auto n = std::dynamic_pointer_cast<StringLiteral>(node)){
        return std::make_shared<String>(n->Value);
    } else if (auto n = std::dynamic_pointer_cast<Boolean>(node)){
        return nativeBoolToBooleanObject(n->Value);
    } else if (auto n = std::dynamic_pointer_cast<PrefixExpression>(node)){
        auto right = Eval(n->Right, env);
        if(isError(right)) {
            return right;
        }
        return evalPrefixExpression(n->Operator, right);
    } else if (auto n = std::dynamic_pointer_cast<InfixExpression>(node)){
        auto left = Eval(n->Left, env);
        if(isError(left)){
            return left;
        }

        auto right = Eval(n->Right, env);
        if(isError(right)) {
            return right;
        }
        return evalInfixExpression(n->Operator, left, right);
    } else if (auto n = std::dynamic_pointer_cast<IfExpression>(node)){
        return evalIfExpression(n, env);
    } else if (auto n = std::dynamic_pointer_cast<Identifier>(node)){
        return evalIdentifier(n, env);
    } else if (auto n = std::dynamic_pointer_cast<FunctionLiteral>(node)){
        auto params = n->Parameters;
        auto body = n->Body;
        return std::make_shared<Function>(params, env, body);
    } else if (auto n = std::dynamic_pointer_cast<CallExpression>(node)){
        auto function = Eval(n->Function, env);
        
        if(isError(function)){
            return function;
        }

        auto args = evalExpressions(n->Arguments, env);
        if(args.size() == 1 && isError(args[0])){
            return args[0];
        }
        return applyFunction(function, args);
    } else if (auto n = std::dynamic_pointer_cast<ArrayLiteral>(node)){
        auto elements = evalExpressions(n->Elements, env);
        if(elements.size() == 1 && isError(elements[0])) return elements[0];
        return std::make_shared<ArrayObject>(elements);
    } else if (auto n = std::dynamic_pointer_cast<IndexExpression>(node)) {
        
    }


    
    return nullptr;
}

std::shared_ptr<Object> Evaluator::evalProgram(std::shared_ptr<Program> program, std::shared_ptr<Environment> env){
    std::shared_ptr<Object> result;

    for(auto& stmt : program->Statements){
        result = Eval(stmt, env);
        if(auto returnValue = std::dynamic_pointer_cast<ReturnValue>(result)){
            return returnValue->Value;
        }
        else if(auto errorValue = std::dynamic_pointer_cast<Error>(result)){
            return errorValue;
        }
    }

    return result;
}

std::shared_ptr<Object> Evaluator::evalBlockStatement(std::shared_ptr<BlockStatement> block, std::shared_ptr<Environment> env){
    std::shared_ptr<Object> result;

    for(auto& stmt: block->Statements) {
        result = Eval(stmt, env);
        if(result){
            auto rt = result->Type();
            if(rt == RETURN_VALUE_OBJ or rt == ERROR_OBJ){
                return result;
            }
        }
    }

    return result;
}

std::shared_ptr<BooleanObject> Evaluator::nativeBoolToBooleanObject(bool input){
    return input ? ObjectConstants::TRUE : ObjectConstants::FALSE;
}

std::shared_ptr<Object> Evaluator::evalPrefixExpression(const std::string& op, std::shared_ptr<Object> right){
    if(op == "!"){
        return evalBangOperatorExpression(right);
    }
    else if(op == "-"){
        return evalMinusPrefixOperatorExpression(right);
    }
    else{
        return newError("unknown operator: %s%s", op.c_str(), ObjectTypeToString(right->Type()).c_str());
    }
}

std::shared_ptr<Object> Evaluator::evalInfixExpression(const std::string& op, std::shared_ptr<Object> left, std::shared_ptr<Object> right){
    if (left->Type() == INTEGER_OBJ && right->Type() == INTEGER_OBJ) {
        return evalIntegerInfixExpression(op, left, right);
    } else if (left->Type() != right->Type()) {
        return newError("type mismatch: %s %s %s", ObjectTypeToString(left->Type()).c_str(), op.c_str(), ObjectTypeToString(right->Type()).c_str());
    } else if (op == "==") {
        return nativeBoolToBooleanObject(left == right);
    } else if (op == "!=") {
        return nativeBoolToBooleanObject(left != right);
    } else {
        return newError("unknown operator: %s %s %s", ObjectTypeToString(left->Type()).c_str(), op.c_str(), ObjectTypeToString(right->Type()).c_str());
    }
}

std::shared_ptr<Object> Evaluator::evalBangOperatorExpression(std::shared_ptr<Object> right){
    if(right == ObjectConstants::TRUE){
        return ObjectConstants::FALSE;
    }
    else if(right == ObjectConstants::FALSE){
        return ObjectConstants::TRUE;
    }
    else if(right == ObjectConstants::NULL_OBJ){
        return ObjectConstants::TRUE;
    }
    else{
        return ObjectConstants::FALSE;
    }
}

std::shared_ptr<Object> Evaluator::evalMinusPrefixOperatorExpression(std::shared_ptr<Object> right){
    if(right->Type() != INTEGER_OBJ){
        return newError("unknown operator: -%s", ObjectTypeToString(right->Type()).c_str());
    }

    int value = std::static_pointer_cast<Integer>(right)->Value;
    return std::make_shared<Integer>(-value);
}

std::shared_ptr<Object> Evaluator::evalIntegerInfixExpression(const std::string& op, std::shared_ptr<Object> left, std::shared_ptr<Object> right){
    int leftVal = std::static_pointer_cast<Integer>(left)->Value;
    int rightVal = std::static_pointer_cast<Integer>(right)->Value;

    if(op == "+") { return std::make_shared<Integer>(leftVal + rightVal);}
    else if (op == "-") { return std::make_shared<Integer>(leftVal - rightVal); }
    else if (op == "*") { return std::make_shared<Integer>(leftVal * rightVal); }
    else if (op == "/") { return std::make_shared<Integer>(leftVal / rightVal); }
    else if (op == "<") { return nativeBoolToBooleanObject(leftVal < rightVal); }
    else if (op == ">") { return nativeBoolToBooleanObject(leftVal > rightVal); }
    else if (op == "==") { return nativeBoolToBooleanObject(leftVal == rightVal); }
    else if (op == "!=") { return nativeBoolToBooleanObject(leftVal != rightVal); }
    else {return newError("unknown operator: %s %s %s", ObjectTypeToString(left->Type()).c_str(), op.c_str(), ObjectTypeToString(right->Type()).c_str()); }
    //else {return newError("unknown operator: %s %s %s", left->Inspect().c_str(), op.c_str(), right->Inspect().c_str()); }
}

std::shared_ptr<Object> Evaluator::evalIfExpression(std::shared_ptr<IfExpression> ie, std::shared_ptr<Environment> env){
    auto condition = Eval(ie->Condition, env);
    if(isError(condition)) return condition;
    if(isTruthy(condition)){
        return Eval(ie->Consequence, env);
    }
    else if(ie->Alternative){
        return Eval(ie->Alternative, env);
    }
    else{
        return ObjectConstants::NULL_OBJ;
    }
}

std::shared_ptr<Object> Evaluator::evalIdentifier(std::shared_ptr<Identifier> node, std::shared_ptr<Environment> env){
    auto val = env->Get(node->Value());
    if(!val) return newError("identifier not found: " + node->Value());
    return val;
}

bool Evaluator::isTruthy(std::shared_ptr<Object> obj){
    if(obj == ObjectConstants::NULL_OBJ) return false;
    else if(obj == ObjectConstants::TRUE) return true;
    else if(obj == ObjectConstants::FALSE) return false;
    else return true;
}

//The ellipsis ... in the parameter list of a function, like the 
//newError function in Go or C++, is used to denote a variadic 
//function. This means that the function can accept an arbitrary 
//number of arguments of a specified type. 
std::shared_ptr<Error> Evaluator::newError(const std::string format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    va_end(args);
    return std::make_shared<Error>(buffer);
}


bool Evaluator::isError(std::shared_ptr<Object> obj){
    if(obj) return obj->Type() == ERROR_OBJ;
    return false;
}

std::vector<std::shared_ptr<Object>> Evaluator::evalExpressions(std::vector<std::shared_ptr<Expression>> exps, std::shared_ptr<Environment> env){
    std::vector<std::shared_ptr<Object>> result;
    for (auto& exp : exps) {
        auto evaluated = Eval(exp, env);
        if (isError(evaluated)) {
            // If an error occurs, return a vector with just that error.
            return {evaluated};
        }
        result.push_back(evaluated);
    }
    return result;
}

std::shared_ptr<Object> Evaluator::applyFunction(std::shared_ptr<Object> fn, std::vector<std::shared_ptr<Object>> args){
    auto function = std::dynamic_pointer_cast<Function>(fn);
    if (!function) {
        // Not a function object, return an error.
        return newError("not a function: %s", fn->Inspect().c_str());
    }

    auto extendedEnv = extendFunctionEnv(function, args);
    auto evaluated = Eval(function->Body, extendedEnv);
    return unwrapReturnValue(evaluated);
}

std::shared_ptr<Environment> Evaluator::extendFunctionEnv(std::shared_ptr<Function> fn, std::vector<std::shared_ptr<Object>> args){
    auto env = std::make_shared<Environment>(fn->Env);
    for (size_t i = 0; i < fn->Parameters.size(); ++i) {
        env->Set(fn->Parameters[i]->Value(), args[i]);
    }
    return env;
}

std::shared_ptr<Object> Evaluator::unwrapReturnValue(std::shared_ptr<Object> obj){
    auto returnValue = std::dynamic_pointer_cast<ReturnValue>(obj);
    if (returnValue) {
        return returnValue->Value;
    }
    return obj;
}

//g++ -std=c++17 -Isrc -c src/monkey/evaluator/evaluator.cpp -o evaluator.o