#include "repl/repl.hpp"
#include <iostream>

int main() {
    std::cout << "This is the Monkey programming language!" << std::endl;
    std::cout << "Feel free to type in commands" << std::endl;

    // Start the REPL using the standard input and output.
    REPL::Start(std::cin, std::cout);

    return 0;
}

//g++ -std=c++17 -I. -o monkey_repl main.cpp repl/repl.cpp lexer/lexer.cpp parser/parser.cpp ast/ast.cpp token/token.cpp && ./monkey_repl
