#include "repl.hpp"
#include <sstream>
#include <cassert>

int main() {
    // This stringstream will simulate the input for the REPL.
    std::stringstream input;
    input << "let x = 5;\n";
    input << "let y = 10;\n";
    input << "x + y;\n";

    // This stringstream will capture the REPL's output.
    std::stringstream output;

    // Start the REPL with the mock input and output.
    REPL::Start(input, output);

    // Here, you can check the output to ensure it's what you expect.
    // For now, I'll just check if it contains certain token outputs.
    // Depending on your REPL's verbosity, you might want to adjust these checks.
    std::string replOutput = output.str();
    std::cout << "REPL Output: " << replOutput << std::endl;

    
    assert(replOutput.find("LET") != std::string::npos);
    assert(replOutput.find("x") != std::string::npos);
    assert(replOutput.find("5") != std::string::npos);
    assert(replOutput.find("+") != std::string::npos);
    assert(replOutput.find("y") != std::string::npos);
    assert(replOutput.find("10") != std::string::npos);

    std::cout << "All tests passed!" << std::endl;
    return 0;
}

//g++ -std=c++17 -Isrc -o repl_test src/monkey/repl/repl.cpp src/monkey/lexer/lexer.cpp src/monkey/token/token.cpp src/monkey/repl/repl_test.cpp && ./repl_test

