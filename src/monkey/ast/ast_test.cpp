#include "ast.hpp"
#include <iostream>

//AST Test: This tests the construction of an Abstract Syntax Tree (AST) and its string representation.
using namespace YOXS_AST;

void TestString() {
    Program program;

    // Setting up the LetStatement
    auto letStatement = std::make_unique<LetStatement>();
    letStatement->token = Token{TokenType::LET, "let"};

    // Setting up the Name Identifier
    auto name = std::make_unique<Identifier>();
    name->token = Token{TokenType::IDENT, "myVar"};
    letStatement->Name = std::move(name);

    // Setting up the Value Identifier
    auto value = std::make_unique<Identifier>();
    value->token = Token{TokenType::IDENT, "anotherVar"};
    letStatement->Value = std::move(value);

    program.Statements.push_back(std::move(letStatement));

    if (program.String() != "let myVar = anotherVar;") {
        std::cerr << "program.String() is wrong. got: " << program.String() << std::endl;
        exit(1);
    } else {
        std::cout << "TestString passed!" << std::endl;
    }
}

int main() {
    TestString();
    return 0;
}

//g++ -std=c++17 -Isrc -o ast_test src/monkey/ast/ast.cpp src/monkey/token/token.cpp src/monkey/ast/ast_test.cpp && ./ast_test
