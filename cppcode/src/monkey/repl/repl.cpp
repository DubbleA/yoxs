#include "repl.hpp"

const std::string PROMPT = ">> ";

void REPL::Start(std::istream& in, std::ostream& out) {
    std::string line;

    while (true) {
        out << PROMPT;
        if (!std::getline(in, line)) {
            return; // Exit if there's an error or EOF is encountered
        }

        Lexer l(line);

        for (Token tok = l.NextToken(); tok.Type != TokenType::EOF_TOKEN; tok = l.NextToken()) {
            out << TokenTypeToString(tok.Type) << ": " << tok.Literal << "\n";
        }
    }
}
