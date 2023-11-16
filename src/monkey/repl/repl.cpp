#include "repl.hpp"

const std::string PROMPT = ">> ";

void REPL::tokenStart(std::istream& in, std::ostream& out){
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

void REPL::parserStart(std::istream& in, std::ostream& out){
    std::string line;

    while (true) {
        out << PROMPT;
        if (!std::getline(in, line)) {
            return; // Exit if there's an error or EOF is encountered
        }

        Lexer l(line);
        Parser p(l);
        
        auto program = p.ParseProgram();
        if (!p.Errors().empty()) {
            printParserErrors(out, p.Errors());
            continue;
        }

        out << program->String();
        out << "\n";
    }
}   

void REPL::Start(std::istream& in, std::ostream& out) {
    std::string line;

    while (true) {
        out << PROMPT;
        if (!std::getline(in, line)) {
            return; // Exit if there's an error or EOF is encountered
        }

        Lexer l(line);
        Parser p(l);
        
        auto program = p.ParseProgram();
        if (!p.Errors().empty()) {
            printParserErrors(out, p.Errors());
            continue;
        }

        auto env = std::make_shared<Environment>();
        Evaluator evaluator;
        auto evaluated = evaluator.Eval(program, env);
        if(evaluated) {
            out << evaluated->Inspect() << "\n";
        }
    }
}

void REPL::StartSingle(std::istream& in, std::ostream& out){
    std::string line;
    
    out << PROMPT;
    if (!std::getline(in, line)) {
        return; // Exit if there's an error or EOF is encountered
    }
    out << "Input: " << line << "\n";
    Lexer l(line);
    Lexer lPrint(line);
    out << "Tokens:\n";
    for (Token tok = lPrint.NextToken(); tok.Type != TokenType::EOF_TOKEN; tok = lPrint.NextToken()) {
        out << TokenTypeToString(tok.Type) << ": " << tok.Literal << "\n";
    }
    Parser p(l);
        
    auto program = p.ParseProgram();
    if (!p.Errors().empty()) {
        printParserErrors(out, p.Errors());
    }

    out << "Program: " << program->String() << "\n";

    auto env = std::make_shared<Environment>();
    Evaluator evaluator;
    auto evaluated = evaluator.Eval(program, env);
    if(evaluated) {
        out << "Evaluated: " << evaluated->Inspect() << "\n";
    }
    
}

void REPL::printParserErrors(std::ostream& out, const std::vector<std::string>& errors) {
    out << "Woops! We ran into an error:\n";
    for (const auto& msg : errors) {
        out << "\t" << msg << "\n";
    }
}
