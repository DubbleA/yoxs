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
bool testNullObject(std::shared_ptr<Object> obj);

/*
struct TestCase {
    std::string input;
    std::string expectedIdentifier;
    std::variant<int, bool, std::string> expectedValue; // Assumes all expected values are int or bool or string.
};

std::vector<TestCase> tests = {
    {"let x = 5;", "x", 5},
    {"let y = true;", "y", true},
    {"let foobar = y;", "foobar", "y"}
};
*/

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
}
void TestErrorHandling();
void TestLetStatements();
void TestFunctionObject();
void TestFunctionApplication();
void TestEnclosingEnvironments();
std::shared_ptr<Object> testEval(const std::string& input);
bool testIntegerObject(const std::shared_ptr<Object>& obj, int64_t expected);
bool testBooleanObject(const std::shared_ptr<Object>& obj, bool expected);
bool testNullObject(std::shared_ptr<Object> obj);