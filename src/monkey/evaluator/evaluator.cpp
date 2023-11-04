#include "evaluator.hpp"

std::shared_ptr<Null> ObjectConstants::NULL_OBJ = std::make_shared<Null>();
std::shared_ptr<Boolean> ObjectConstants::TRUE = std::make_shared<Boolean>(true);
std::shared_ptr<Boolean> ObjectConstants::FALSE = std::make_shared<Boolean>(false);


std::shared_ptr<Object> Evaluator::Eval(NodeVariant node, std::shared_ptr<Environment> env) {
    return std::visit(EvaluatorVisitor{env}, node);
}
// std::shared_ptr<Object> Evaluator::Eval(std::shared_ptr<Node> node, std::shared_ptr<Environment> env){
//     // auto nodeType = std::dynamic_pointer_cast<decltype(node)>(node);

//     // switch(nodeType){

//     // }
//     if(auto nodeType = std::dynamic_pointer_cast<Program>(node)){
//         return evalProgram(nodeType, env);
//     }
//     else if(auto nodeType = std::dynamic_pointer_cast<BlockStatement>(node)){
//         return evalBlockStatement(nodeType, env);
//     }
//     else if(auto nodeType = std::dynamic_pointer_cast<ExpressionStatement>(node)){
//         return Eval(std::shared_ptr<Node>(std::move(nodeType->expr)), env);
//     }
//     else if(auto nodeType = std::dynamic_pointer_cast<ReturnStatement>(node)){
//         auto val = Eval(std::shared_ptr<Node>(std::move(nodeType->ReturnValue)), env);
//         if(isError(val)) {
//             return val;
//         }
//         return std::make_shared<ReturnValue>(val);
//     }
//     else if(auto nodeType = std::dynamic_pointer_cast<LetStatement>(node)){
//         auto val = Eval(std::shared_ptr<Node>(std::move(nodeType->Value)), env);
//         if(isError(val)) {
//             return val;
//         }
//         env->Set(nodeType->Name->Value(), val);
//     }
//     //Expressions
//     else if(auto note)

// }