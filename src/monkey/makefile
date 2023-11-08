# src/monkey/Makefile

include ../../common.mk

AST_DIR := ast
EVALUATOR_DIR := evaluator
LEXER_DIR := lexer
PARSER_DIR := parser
TOKEN_DIR := token
REPL_DIR := repl
OBJECT_DIR := object

.PHONY: all build clean tests token_test lexer_test repl_test ast_test parser_test evaluator_test

all: build tests

build:
	@echo "Build commands for monkey components"

tests: token_test lexer_test repl_test ast_test parser_test evaluator_test

token_test:
	$(CXX) $(CXXFLAGS) -I. $(TOKEN_DIR)/token_test.cpp $(TOKEN_DIR)/token.cpp -o token_test.out
	./token_test.out

lexer_test:
	$(CXX) $(CXXFLAGS) -I. $(LEXER_DIR)/lexer_test.cpp $(LEXER_DIR)/lexer.cpp $(TOKEN_DIR)/token.cpp -o lexer_test.out
	./lexer_test.out

repl_test:
	$(CXX) $(CXXFLAGS) -I. $(REPL_DIR)/repl_test.cpp $(REPL_DIR)/repl.cpp $(LEXER_DIR)/lexer.cpp $(TOKEN_DIR)/token.cpp $(PARSER_DIR)/parser.cpp $(AST_DIR)/ast.cpp $(EVALUATOR_DIR)/evaluator.cpp $(OBJECT_DIR)/environment.cpp $(OBJECT_DIR)/object.cpp -o repl_test.out
	./repl_test.out

ast_test:
	$(CXX) $(CXXFLAGS) -I. $(AST_DIR)/ast_test.cpp $(AST_DIR)/ast.cpp $(TOKEN_DIR)/token.cpp -o ast_test.out
	./ast_test.out

parser_test:
	$(CXX) $(CXXFLAGS) -I. $(PARSER_DIR)/parser_test.cpp $(PARSER_DIR)/parser.cpp $(LEXER_DIR)/lexer.cpp $(TOKEN_DIR)/token.cpp $(AST_DIR)/ast.cpp -o parser_test.out
	./parser_test.out

evaluator_test:
	$(CXX) $(CXXFLAGS) -I. -c $(PARSER_DIR)/parser.cpp -o parser.o
	$(CXX) $(CXXFLAGS) -I. -c $(LEXER_DIR)/lexer.cpp -o lexer.o
	$(CXX) $(CXXFLAGS) -I. -c $(TOKEN_DIR)/token.cpp -o token.o
	$(CXX) $(CXXFLAGS) -I. -c $(AST_DIR)/ast.cpp -o ast.o
	$(CXX) $(CXXFLAGS) -I. -c $(OBJECT_DIR)/object.cpp -o object.o
	$(CXX) $(CXXFLAGS) -I. -c $(OBJECT_DIR)/environment.cpp -o environment.o
	$(CXX) $(CXXFLAGS) -I. -c $(EVALUATOR_DIR)/evaluator.cpp -o evaluator.o
	$(CXX) $(CXXFLAGS) -I. -c $(EVALUATOR_DIR)/evaluator_test.cpp -o evaluator_test.o
	$(CXX) -o evaluator_test.out parser.o lexer.o token.o ast.o object.o environment.o evaluator.o evaluator_test.o
	./evaluator_test.out

clean:
	rm -f *.out *.o
