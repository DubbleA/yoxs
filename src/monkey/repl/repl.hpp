#ifndef REPL_H
#define REPL_H

#include <iostream>
#include <string>
#include "../lexer/lexer.hpp"
#include "../token/token.hpp"

class REPL {
public:
    static void Start(std::istream& in, std::ostream& out);
};

#endif // REPL_H
