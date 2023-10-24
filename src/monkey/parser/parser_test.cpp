#include "parser.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <cassert>
#include <variant>

void TestLetStatements() {
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

    for (const auto& tt : tests) {
        Lexer l(tt.input);
        Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        if (program->Statements.size() != 1) {
            std::cerr << "program.Statements does not contain 1 statements. got=" 
                      << program->Statements.size() << std::endl;
            return; // This was a fatal error in Go, so we just return here.
        }

        if (!testLetStatement(program->Statements[0], tt.expectedIdentifier)) {
            return;
        }

        // Here, we'll directly get the value from the statement and test it.
        const Expression& value = *dynamic_cast<LetStatement*>(program->Statements[0].get())->Value;

        // if(auto *ptr = dynamic_cast<LetStatement*>(program->Statements[0].get())){
        // //use ptr
        // }

        #TODO FIX THE ABOVE
        if (!testLiteralExpression(value, tt.expectedValue)) {
            return;
        }
    }
}

void TestReturnStatements() {
    struct TestCase {
        std::string input;
        std::variant<int, bool, std::string> expectedValue;
    };

    std::vector<TestCase> tests = {
        {"return 5;", 5},
        {"return true;", true},
        {"return foobar;", "foobar"},
    };

    for (const auto& tt : tests) {
        Lexer l(tt.input);
        Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        assert(program->Statements.size() == 1);

        const auto& stmt = program->Statements[0];
        const auto* returnStmt = dynamic_cast<ReturnStatement*>(stmt.get());
        if (!returnStmt) {
            std::cerr << "stmt not a ReturnStatement. Got " << typeid(*stmt).name() << std::endl;
            return;
        }

        assert(returnStmt->TokenLiteral() == "return");

        const Expression& value = *returnStmt->ReturnValue;
        if (!testLiteralExpression(value, tt.expectedValue)) {
            return;
        }
    }
}

void TestIdentifierExpression() {
    const std::string input = "foobar;";

    Lexer l(input);
    Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    assert(program->Statements.size() == 1);

    const auto* exprStmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    if (!exprStmt) {
        std::cerr << "program.Statements[0] is not ExpressionStatement. Got "
                  << typeid(*program->Statements[0]).name() << std::endl;
        return;
    }

    const auto* ident = dynamic_cast<Identifier*>(exprStmt->expr.get());
    if (!ident) {
        std::cerr << "exp not Identifier. Got "
                  << typeid(*exprStmt->expr).name() << std::endl;
        return;
    }

    assert(ident->Value() == "foobar"); // Ensure Identifier has a Value method or appropriate member variable
    assert(ident->TokenLiteral() == "foobar");
}

void TestIntegerLiteralExpression() {
    const std::string input = "5;";

    Lexer l(input);
    Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    assert(program->Statements.size() == 1);

    const auto* exprStmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    if (!exprStmt) {
        std::cerr << "program.Statements[0] is not ExpressionStatement. Got "
                  << typeid(*program->Statements[0]).name() << std::endl;
        return;
    }

    const auto* literal = dynamic_cast<IntegerLiteral*>(exprStmt->expr.get());
    if (!literal) {
        std::cerr << "exp not IntegerLiteral. Got "
                  << typeid(*exprStmt->expr).name() << std::endl;
        return;
    }

    assert(literal->Value == 5); // Ensure IntegerLiteral has a Value member
    assert(literal->TokenLiteral() == "5");
}



void TestParsingPrefixExpressions() {

    struct PrefixTest {
        std::string input;
        std::string oper;
        std::variant<int, bool, std::string> value;
    };

    std::vector<PrefixTest> prefixTests = {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
        {"!foobar;", "!", "foobar"},
        {"-foobar;", "-", "foobar"},
        {"!true;", "!", true},
        {"!false;", "!", false},
    };

    for (const auto& tt : prefixTests) {
        Lexer l(tt.input);
        Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        assert(program->Statements.size() == 1);

        const auto* exprStmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
        if (!exprStmt) {
            std::cerr << "program.Statements[0] is not ExpressionStatement. Got "
                      << typeid(*program->Statements[0]).name() << std::endl;
            return;
        }

        const auto* exp = dynamic_cast<PrefixExpression*>(exprStmt->expr.get());
        if (!exp) {
            std::cerr << "stmt is not PrefixExpression. Got "
                      << typeid(*exprStmt->expr).name() << std::endl;
            return;
        }

        assert(exp->Operator == tt.oper);

        if (!testLiteralExpression(*(exp->Right), tt.value)) {
            return;
        }

    }
}

void TestParsingInfixExpressions() {

    struct InfixTest {
        std::string input;
        std::variant<int, std::string, bool> leftValue;
        std::string oper;
        std::variant<int, std::string, bool> rightValue;
    };

    std::vector<InfixTest> infixTests = {
        {"5 + 5;", 5, "+", 5},
		{"5 - 5;", 5, "-", 5},
		{"5 * 5;", 5, "*", 5},
		{"5 / 5;", 5, "/", 5},
		{"5 > 5;", 5, ">", 5},
		{"5 < 5;", 5, "<", 5},
		{"5 == 5;", 5, "==", 5},
		{"5 != 5;", 5, "!=", 5},
		{"foobar + barfoo;", "foobar", "+", "barfoo"},
		{"foobar - barfoo;", "foobar", "-", "barfoo"},
		{"foobar * barfoo;", "foobar", "*", "barfoo"},
		{"foobar / barfoo;", "foobar", "/", "barfoo"},
		{"foobar > barfoo;", "foobar", ">", "barfoo"},
		{"foobar < barfoo;", "foobar", "<", "barfoo"},
		{"foobar == barfoo;", "foobar", "==", "barfoo"},
		{"foobar != barfoo;", "foobar", "!=", "barfoo"},
		{"true == true", true, "==", true},
		{"true != false", true, "!=", false},
		{"false == false", false, "==", false}
    };

    for (const auto& tt : infixTests) {
        Lexer l(tt.input);
        Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        assert(program->Statements.size() == 1);

        const auto* exprStmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
        if (!exprStmt) {
            std::cerr << "program.Statements[0] is not ExpressionStatement. Got "
                      << typeid(*program->Statements[0]).name() << std::endl;
            return;
        }

        if (!testInfixExpression(exprStmt->expr, tt.leftValue, tt.oper, tt.rightValue)) {
            return;
        }
    }
}

void TestOperatorPrecedenceParsing() {
    struct TestCase {
        std::string input;
        std::string expected;
    };

    std::vector<TestCase> tests = {
        {
			"-a * b",
			"((-a) * b)",
		},
		{
			"!-a",
			"(!(-a))",
		},
		{
			"a + b + c",
			"((a + b) + c)",
		},
		{
			"a + b - c",
			"((a + b) - c)",
		},
		{
			"a * b * c",
			"((a * b) * c)",
		},
		{
			"a * b / c",
			"((a * b) / c)",
		},
		{
			"a + b / c",
			"(a + (b / c))",
		},
		{
			"a + b * c + d / e - f",
			"(((a + (b * c)) + (d / e)) - f)",
		},
		{
			"3 + 4; -5 * 5",
			"(3 + 4)((-5) * 5)",
		},
		{
			"5 > 4 == 3 < 4",
			"((5 > 4) == (3 < 4))",
		},
		{
			"5 < 4 != 3 > 4",
			"((5 < 4) != (3 > 4))",
		},
		{
			"3 + 4 * 5 == 3 * 1 + 4 * 5",
			"((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
		},
		{
			"true",
			"true",
		},
		{
			"false",
			"false",
		},
		{
			"3 > 5 == false",
			"((3 > 5) == false)",
		},
		{
			"3 < 5 == true",
			"((3 < 5) == true)",
		},
		{
			"1 + (2 + 3) + 4",
			"((1 + (2 + 3)) + 4)",
		},
		{
			"(5 + 5) * 2",
			"((5 + 5) * 2)",
		},
		{
			"2 / (5 + 5)",
			"(2 / (5 + 5))",
		},
		{
			"(5 + 5) * 2 * (5 + 5)",
			"(((5 + 5) * 2) * (5 + 5))",
		},
		{
			"-(5 + 5)",
			"(-(5 + 5))",
		},
		{
			"!(true == true)",
			"(!(true == true))",
		},
		{
			"a + add(b * c) + d",
			"((a + add((b * c))) + d)",
		},
		{
			"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
			"add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
		},
		{
			"add(a + b + c * d / f + g)",
			"add((((a + b) + ((c * d) / f)) + g))",
		}
    };

    for (const auto& tt : tests) {
        Lexer l(tt.input);
        Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);  // Make sure this function doesn't require the 't' parameter

        std::string actual = program->String();  // Assuming your AST nodes have a ToString method
        if (actual != tt.expected) {
            std::cerr << "expected=" << tt.expected << ", got=" << actual << std::endl;
        }
    }
}

void TestBooleanExpression() {
    struct TestCase {
        std::string input;
        bool expectedBoolean;
    };

    std::vector<TestCase> tests = {
        {"true;", true},
        {"false;", false},
    };

    for (const auto& tt : tests) {
        Lexer l(tt.input);
        Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        if (program->Statements.size() != 1) {
            std::cerr << "program.Statements does not contain 1 statements. got=" 
                      << program->Statements.size() << std::endl;
            return; // This was a fatal error in Go, so we just return here.
        }

        auto* exprStmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
        if (!exprStmt) {
            std::cerr << "program.Statements[0] is not ExpressionStatement. Got "
                      << typeid(*program->Statements[0]).name() << std::endl;
            return;
        }

        auto* boolean = dynamic_cast<Boolean*>(exprStmt->expr.get());
        if (!boolean) {
            std::cerr << "exp not Boolean. Got "
                      << typeid(*exprStmt->expr).name() << std::endl;
            return;
        }

        if (boolean->Value != tt.expectedBoolean) {
            std::cerr << "boolean.Value not " << tt.expectedBoolean
                      << ". got=" << boolean->Value << std::endl;
        }
    }
}

void TestIfExpression() {
    std::string input = "if (x < y) { x }";

    Lexer l(input);
    Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    if (program->Statements.size() != 1) {
        std::cerr << "program.Statements does not contain 1 statements. got=" 
                  << program->Statements.size() << std::endl;
        return;
    }

    auto* stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    if (!stmt) {
        std::cerr << "program.Statements[0] is not ExpressionStatement. got=" 
                  << typeid(program->Statements[0].get()).name() << std::endl;
        return;
    }

    auto* exp = dynamic_cast<IfExpression*>(stmt->expr.get());
    if (!exp) {
        std::cerr << "stmt.Expression is not IfExpression. got=" 
                  << typeid(stmt->expr.get()).name() << std::endl;
        return;
    }

    if (!testInfixExpression(exp->Condition, "x", "<", "y")) {
        return;
    }

    if (exp->Consequence->Statements.size() != 1) {
        std::cerr << "consequence is not 1 statements. got=" 
                  << exp->Consequence->Statements.size() << std::endl;
        return;
    }

    auto* consequence = dynamic_cast<ExpressionStatement*>(exp->Consequence->Statements[0].get());
    if (!consequence) {
        std::cerr << "Statements[0] is not ExpressionStatement. got=" 
                  << typeid(exp->Consequence->Statements[0].get()).name() << std::endl;
        return;
    }

    if (!testIdentifier(consequence->expr, "x")) {
        return;
    }

    if (exp->Alternative) {
        std::cerr << "exp.Alternative.Statements was not nullptr. got some content." << std::endl;
        return;
    }
}

void TestFunctionLiteralParsing() {
    std::string input = "fn(x, y) { x + y; }";

    Lexer l(input);
    Parser p(l);
    std::unique_ptr<Program> program = p.ParseProgram();
    checkParserErrors(p);

    if (program->Statements.size() != 1) {
        std::cerr << "program.Statements does not contain 1 statements. got=" 
                  << program->Statements.size() << std::endl;
        return;
    }

    auto* exprStmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    if (!exprStmt) {
        std::cerr << "program.Statements[0] is not ExpressionStatement. Got " 
                  << typeid(*program->Statements[0]).name() << std::endl;
        return;
    }

    auto* function = dynamic_cast<FunctionLiteral*>(exprStmt->expr.get());
    if (!function) {
        std::cerr << "stmt.Expression is not FunctionLiteral. Got "
                  << typeid(*exprStmt->expr).name() << std::endl;
        return;
    }

    if (function->Parameters.size() != 2) {
        std::cerr << "function literal parameters wrong. want 2, got="
                  << function->Parameters.size() << std::endl;
        return;
    }

    testLiteralExpression(function->Parameters[0].get(), "x");
    testLiteralExpression(function->Parameters[1].get(), "y");

    if (function->Body->Statements.size() != 1) {
        std::cerr << "function.Body.Statements has not 1 statements. got=" 
                  << function->Body->Statements.size() << std::endl;
        return;
    }

    auto* bodyStmt = dynamic_cast<ExpressionStatement*>(function->Body->Statements[0].get());
    if (!bodyStmt) {
        std::cerr << "function body stmt is not ExpressionStatement. Got "
                  << typeid(*function->Body->Statements[0]).name() << std::endl;
        return;
    }

    testInfixExpression(bodyStmt->expr.get(), "x", "+", "y");
}

void TestFunctionParameterParsing() {
    struct Test {
        std::string input;
        std::vector<std::string> expectedParams;
    };

    std::vector<Test> tests = {
        {"fn() {};", {}},
        {"fn(x) {};", {"x"}},
        {"fn(x, y, z) {};", {"x", "y", "z"}}
    };

    for (const auto& tt : tests) {
        Lexer l(tt.input);
        Parser p(l);
        std::unique_ptr<Program> program = p.ParseProgram();
        checkParserErrors(p); // Assuming this function is modified for C++

        auto* stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
        if (!stmt) {
            std::cerr << "Statement is not ExpressionStatement." << std::endl;
            return;
        }

        auto* function = dynamic_cast<FunctionLiteral*>(stmt->expr.get());
        if (!function) {
            std::cerr << "Expression is not FunctionLiteral." << std::endl;
            return;
        }

        if (function->Parameters.size() != tt.expectedParams.size()) {
            std::cerr << "length parameters wrong. want " 
                      << tt.expectedParams.size() 
                      << ", got=" << function->Parameters.size() << std::endl;
        }

        for (size_t i = 0; i < tt.expectedParams.size(); ++i) {
            testLiteralExpression(function->Parameters[i].get(), tt.expectedParams[i]);
        }
    }
}

#include <iostream>
#include <memory>

// Assuming necessary header files for your parser classes are included

void TestCallExpressionParsing() {
    std::string input = "add(1, 2 * 3, 4 + 5);";

    Lexer l(input);
    Parser p(l);
    std::unique_ptr<Program> program = p.ParseProgram();
    checkParserErrors(p); // Assuming this function is modified for C++

    if (program->Statements.size() != 1) {
        std::cerr << "program.Statements does not contain " << 1 
                  << " statements. got=" << program->Statements.size() << std::endl;
        return;
    }

    auto* stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    if (!stmt) {
        std::cerr << "stmt is not ExpressionStatement. got=" 
                  << typeid(program->Statements[0].get()).name() << std::endl;
        return;
    }

    auto* exp = dynamic_cast<CallExpression*>(stmt->expr.get());
    if (!exp) {
        std::cerr << "stmt.Expression is not CallExpression. got=" 
                  << typeid(stmt->expr.get()).name() << std::endl;
        return;
    }

    if (!testIdentifier(exp->Function.get(), "add")) {
        return;
    }

    if (exp->Arguments.size() != 3) {
        std::cerr << "wrong length of arguments. got=" << exp->Arguments.size() << std::endl;
        return;
    }

    testLiteralExpression(exp->Arguments[0].get(), 1);
    testInfixExpression(exp->Arguments[1].get(), 2, "*", 3);
    testInfixExpression(exp->Arguments[2].get(), 4, "+", 5);
}

void TestCallExpressionParameterParsing() {
    struct TestCase {
        std::string input;
        std::string expectedIdent;
        std::vector<std::string> expectedArgs;
    };

    std::vector<TestCase> tests = {
        {"add();", "add", {}},
        {"add(1);", "add", {"1"}},
        {"add(1, 2 * 3, 4 + 5);", "add", {"1", "(2 * 3)", "(4 + 5)"}}
    };

    for (const auto& tt : tests) {
        Lexer l(tt.input);
        Parser p(l);
        std::unique_ptr<Program> program = p.ParseProgram();
        checkParserErrors(p); // Assuming this function is adapted for C++

        auto* stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
        if (!stmt) {
            std::cerr << "First statement is not an ExpressionStatement. got=" 
                      << typeid(program->Statements[0].get()).name() << std::endl;
            return;
        }

        auto* exp = dynamic_cast<CallExpression*>(stmt->expr.get());
        if (!exp) {
            std::cerr << "stmt.Expression is not CallExpression. got=" 
                      << typeid(stmt->expr.get()).name() << std::endl;
            return;
        }

        if (!testIdentifier(exp->Function.get(), tt.expectedIdent)) {
            return;
        }

        if (exp->Arguments.size() != tt.expectedArgs.size()) {
            std::cerr << "wrong number of arguments. want=" << tt.expectedArgs.size() 
                      << ", got=" << exp->Arguments.size() << std::endl;
            return;
        }

        for (size_t i = 0; i < tt.expectedArgs.size(); ++i) {
            if (exp->Arguments[i]->String() != tt.expectedArgs[i]) {
                std::cerr << "argument " << i << " wrong. want=" << tt.expectedArgs[i] 
                          << ", got=" << exp->Arguments[i]->String() << std::endl;
            }
        }
    }
}

void checkParserErrors(const Parser& p) {
    const auto& errors = p.Errors();
    for (const auto& error : errors) {
        std::cerr << "parser error: " << error << std::endl;
    }
    assert(errors.empty());
}

bool testLetStatement(const std::unique_ptr<Statement>& stmt, const std::string& expectedName) {
    assert(stmt->TokenLiteral() == "let");
    const auto* letStmt = dynamic_cast<LetStatement*>(stmt.get());
    if (!letStmt) {
        std::cerr << "stmt not a LetStatement. Got " << typeid(*stmt).name() << std::endl;
        return false;
    }
    assert(letStmt->Name->Value == expectedName);
    assert(letStmt->Name->TokenLiteral() == expectedName);
    return true;
}

int main() {
    TestLetStatements();
    // ... call other test functions ...

    std::cout << "All tests passed!" << std::endl;
    return 0;
}