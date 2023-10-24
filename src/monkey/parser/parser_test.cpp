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



