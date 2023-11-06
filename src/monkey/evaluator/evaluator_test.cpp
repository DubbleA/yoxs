#include <iostream>
#include <cassert>
#include "evaluator.hpp"
#include "../lexer/lexer.hpp"
#include "../object/object.hpp"
#include "../parser/parser.hpp"

void TestEvalIntegerExpression();
void TestEvalBooleanExpression();
void TestBangOperator();
void TestIfElseExpressions();
void TestReturnStatements();
void TestErrorHandling();
void TestLetStatements();
void TestFunctionObject();
void TestFunctionApplication();
void TestEnclosingEnvironments();
std::shared_ptr<Object> testEval(const std::string& input);
bool testIntegerObject(const std::shared_ptr<Object>& obj, int64_t expected);
bool testBooleanObject(const std::shared_ptr<Object>& obj, bool expected);
bool testNullObject(const std::shared_ptr<Object> obj);

void TestEvalIntegerExpression(){
    struct TestCase {
        std::string input;
        int64_t expected;
    };
    std::vector<TestCase> tests = {
        {"5", 5},
		{"10", 10},
		{"-5", -5},
		{"-10", -10},
		{"5 + 5 + 5 + 5 - 10", 10},
		{"2 * 2 * 2 * 2 * 2", 32},
		{"-50 + 100 + -50", 0},
		{"5 * 2 + 10", 20},
		{"5 + 2 * 10", 25},
		{"20 + 2 * -10", 0},
		{"50 / 2 * 2 + 10", 60},
		{"2 * (5 + 10)", 30},
		{"3 * 3 * 3 + 10", 37},
		{"3 * (3 * 3) + 10", 37},
		{"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50}
    };

    for(const auto& tt : tests){
        auto evaluted = testEval(tt.input);
        testIntegerObject(evaluted, tt.expected);
    }
}

void TestEvalBooleanExpression(){
    struct TestCase {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> tests = {
        {"true", true},
		{"false", false},
		{"1 < 2", true},
		{"1 > 2", false},
		{"1 < 1", false},
		{"1 > 1", false},
		{"1 == 1", true},
		{"1 != 1", false},
		{"1 == 2", false},
		{"1 != 2", true},
		{"true == true", true},
		{"false == false", true},
		{"true == false", false},
		{"true != false", true},
		{"false != true", true},
		{"(1 < 2) == true", true},
		{"(1 < 2) == false", false},
		{"(1 > 2) == true", false},
		{"(1 > 2) == false", true},
    };

    for(const auto& tt : tests){
        auto evaluted = testEval(tt.input);
        testBooleanObject(evaluted, tt.expected);
    }
}

void TestBangOperator(){
    struct TestCase {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> tests = {
        {"!true", false},
		{"!false", true},
		{"!5", false},
		{"!!true", true},
		{"!!false", false},
		{"!!5", true},
    };

    for(const auto& tt : tests){
        auto evaluted = testEval(tt.input);
        testBooleanObject(evaluted, tt.expected);
    }
}

void TestIfElseExpressions(){
    struct TestCase {
        std::string input;
        std::shared_ptr<Object> expected;
    };

    std::vector<TestCase> tests = {
        {"if (true) { 10 }", std::make_shared<Integer>(10)},
		{"if (false) { 10 }", nullptr},
		{"if (1) { 10 }", std::make_shared<Integer>(10)},
		{"if (1 < 2) { 10 }", std::make_shared<Integer>(10)},
		{"if (1 > 2) { 10 }", nullptr},
		{"if (1 > 2) { 10 } else { 20 }", std::make_shared<Integer>(20)},
		{"if (1 < 2) { 10 } else { 20 }", std::make_shared<Integer>(10)},
    };

    for(const auto& tt : tests){
        auto evaluted = testEval(tt.input);
        if(tt.expected){
            testIntegerObject(evaluted, std::static_pointer_cast<Integer>(tt.expected)->Value);
        } else {
            testNullObject(evaluted);
        }
    }

}

void TestReturnStatements() {
    struct TestCase {
        std::string input;
        int64_t expected;
    };

    std::vector<TestCase> tests = {
        {"return 10;", 10},
		{"return 10; 9;", 10},
		{"return 2 * 5; 9;", 10},
		{"9; return 2 * 5; 9;", 10},
		{"if (10 > 1) { return 10; }", 10},
        {
            "if (10 > 1) {\n"
            "  if (10 > 1) {\n"
            "    return 10;\n"
            "  }\n"
            "\n"
            "  return 1;\n"
            "}\n",
            10
        },
        {
            "let f = fn(x) {\n"
            "  return x;\n"
            "  x + 10;\n"
            "};\n"
            "f(10);",
            10
        },
        {
            "let f = fn(x) {\n"
            "   let result = x + 10;\n"
            "   return result;\n"
            "   return 10;\n"
            "};\n"
            "f(10);",
            20
        }
    };

    for (const auto& tt : tests){
        auto evaluated = testEval(tt.input);
        testIntegerObject(evaluated, tt.expected);
    }
}

void TestErrorHandling(){
    struct TestCase{
        std::string input;
        std::string expectedMessage;
    };
    std::vector<TestCase> tests = {
        {
            "5 + true;",
            "type mismatch: INTEGER + BOOLEAN"
        },
        {
            "5 + true; 5;",
            "type mismatch: INTEGER + BOOLEAN"
        },
        {
            "-true",
            "unknown operator: -BOOLEAN"
        },
        {
            "true + false;",
            "unknown operator: BOOLEAN + BOOLEAN"
        },
        {
            "true + false + true + false;",
            "unknown operator: BOOLEAN + BOOLEAN"
        },
        {
            "5; true + false; 5",
            "unknown operator: BOOLEAN + BOOLEAN"
        },
        {
            "if (10 > 1) { true + false; }",
            "unknown operator: BOOLEAN + BOOLEAN"
        },
        {
            "if (10 > 1) {\n"
            "  if (10 > 1) {\n"
            "    return true + false;\n"
            "  }\n"
            "\n"
            "  return 1;\n"
            "}\n",
            "unknown operator: BOOLEAN + BOOLEAN"
        },
        {
            "foobar",
            "identifier not found: foobar"
        },
    };

    for (const auto& tt : tests) {
        std::shared_ptr<Object> evaluated = testEval(tt.input);
        std::shared_ptr<Error> errObj = std::dynamic_pointer_cast<Error>(evaluated);
        if (!errObj) {
            auto& evaluated_ref = *evaluated;
            std::cerr << "no error object returned. got=" << typeid(evaluated_ref).name() << "\n";
            continue;
        }

        if (errObj->Message != tt.expectedMessage) {
            std::cerr << "wrong error message. expected=\"" << tt.expectedMessage << "\", got=\"" << errObj->Message << "\"\n";
        }
    }
}
    
void TestLetStatements() {
    struct TestCase {
        std::string input;
        int64_t expected;
    };
    std::vector<TestCase> tests = {
        {"let a = 5; a;", 5},
		{"let a = 5 * 5; a;", 25},
		{"let a = 5; let b = a; b;", 5},
		{"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    };

    for(const auto& tt : tests){
        testIntegerObject(testEval(tt.input), tt.expected);
    }
}

void TestFunctionObject() {
    std::string input = "fn(x) { x + 2; };";

    std::shared_ptr<Object> evaluated = testEval(input);
    std::shared_ptr<Function> fn = std::dynamic_pointer_cast<Function>(evaluated);
    if (!fn) {
        throw std::runtime_error("Object is not Function.");
    }

    if (fn->Parameters.size() != 1) {
        throw std::runtime_error("Function has wrong number of parameters.");
    }

    if (fn->Parameters[0]->String() != "x") {
        throw std::runtime_error("Parameter is not 'x'.");
    }

    std::string expectedBody = "(x + 2)";
    if (fn->Body->String() != expectedBody) {
        throw std::runtime_error("Body is not '" + expectedBody + "'.");
    }
}

void TestFunctionApplication(){
    struct TestCase {
        std::string input;
        int64_t expected;
    };

    std::vector<TestCase> tests = {
        {"let identity = fn(x) { x; }; identity(5);", 5},
		{"let identity = fn(x) { return x; }; identity(5);", 5},
		{"let double = fn(x) { x * 2; }; double(5);", 10},
		{"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
		{"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
		{"fn(x) { x; }(5)", 5},
    };
    
    for(const auto& tt : tests){
        testIntegerObject(testEval(tt.input), tt.expected);
    }
}

void TestEnclosingEnvironments() {
    std::string input = R"(
    let first = 10;
    let second = 10;
    let third = 10;

    let ourFunction = fn(first) {
      let second = 20;

      first + second + third;
    };

    ourFunction(20) + first + second;)";

    testIntegerObject(testEval(input), 70);
}

std::shared_ptr<Object> testEval(const std::string& input) {
    Lexer l(input);
    Parser p(l);
    auto program = p.ParseProgram();
    //std::cout << "input: " << input << " program: " << program->String() << std::endl; 
    auto env = std::make_shared<Environment>();
    Evaluator evaluator;
    return evaluator.Eval(program, env);
}

bool testIntegerObject(const std::shared_ptr<Object>& obj, int64_t expected) {
    auto result = std::dynamic_pointer_cast<Integer>(obj);
    if (!result) {
        std::cerr << "Object is not Integer. Got=" << typeid(obj.get()).name() << std::endl;
        return false;
    }
    if (result->Value != expected) {
        std::cerr << "Object has wrong value. Got=" << result->Value << ", Want=" << expected << std::endl;
        return false;
    }
    return true;
}

bool testBooleanObject(const std::shared_ptr<Object>& obj, bool expected) {
    auto result = std::dynamic_pointer_cast<BooleanObject>(obj);
    if (!result) {
        std::cerr << "Object is not Boolean. Got=" << typeid(obj.get()).name() << std::endl;
        return false;
    }
    if (result->Value != expected) {
        std::cerr << "Object has wrong value. Got=" << result->Value << ", Want=" << expected << std::endl;
        return false;
    }
    return true;
}

bool testNullObject(const std::shared_ptr<Object> obj) {
    if (obj != nullptr && typeid(obj->Type()) != typeid(NULL_OBJ)) {
        std::cerr << "Object is not NULL. Got=" << typeid(obj.get()).name() << std::endl;
        return false;
    }
    return true;
}

int main() {
    TestEvalIntegerExpression();
    TestEvalBooleanExpression();
    TestBangOperator();
    TestIfElseExpressions();
    TestReturnStatements();
    TestErrorHandling();
    TestLetStatements();
    TestFunctionObject();
    TestFunctionApplication();
    TestEnclosingEnvironments();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}

// g++ -std=c++17 -Isrc -c src/monkey/parser/parser.cpp -o parser.o
// g++ -std=c++17 -Isrc -c src/monkey/lexer/lexer.cpp -o lexer.o
// g++ -std=c++17 -Isrc -c src/monkey/token/token.cpp -o token.o
// g++ -std=c++17 -Isrc -c src/monkey/ast/ast.cpp -o ast.o
// g++ -std=c++17 -Isrc -c src/monkey/object/object.cpp -o object.o
// g++ -std=c++17 -Isrc -c src/monkey/object/environment.cpp -o environment.o
// g++ -std=c++17 -Isrc -c src/monkey/evaluator/evaluator.cpp -o evaluator.o
// g++ -std=c++17 -Isrc -c src/monkey/evaluator/evaluator_test.cpp -o evaluator_test.o
// g++ -o evaluator_test parser.o lexer.o token.o ast.o object.o environment.o evaluator.o evaluator_test.o
// ./evaluator_test