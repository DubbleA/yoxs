#ifndef REPL_H
#define REPL_H

#include <iostream>
#include <string>
#include "../lexer/lexer.hpp"
#include "../token/token.hpp"
#include "../parser/parser.hpp"
#include "../ast/ast.hpp"

class REPL {
public:
    static void tokenStart(std::istream& in, std::ostream& out);
    static void Start(std::istream& in, std::ostream& out);
private:
    static void printParserErrors(std::ostream& out, const std::vector<std::string>& errors);
};

#endif // REPL_H
