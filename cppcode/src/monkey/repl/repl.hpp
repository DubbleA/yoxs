#ifndef REPL_H
#define REPL_H

#include <iostream>
#include <string>
#include "monkey/lexer/lexer.hpp"
#include "monkey/token/token.hpp"

class REPL {
public:
    static void Start(std::istream& in, std::ostream& out);
};

#endif // REPL_H
