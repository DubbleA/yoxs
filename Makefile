# Directories
SRC_DIR = src
AST_DIR = $(SRC_DIR)/monkey/ast
LEXER_DIR = $(SRC_DIR)/monkey/lexer
REPL_DIR = $(SRC_DIR)/monkey/repl
TOKEN_DIR = $(SRC_DIR)/monkey/token

# Flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I$(SRC_DIR)

# Default target
all: build all_tests

build:
	@echo "Build done!"

token_test:
	$(CXX) $(CXXFLAGS) $(TOKEN_DIR)/token_test.cpp $(TOKEN_DIR)/token.cpp -o token_test.out
	./token_test.out

lexer_test:
	$(CXX) $(CXXFLAGS) $(LEXER_DIR)/lexer.cpp $(TOKEN_DIR)/token.cpp $(LEXER_DIR)/lexer_test.cpp -o lexer_test.out
	./lexer_test.out

repl_test:
	$(CXX) $(CXXFLAGS) $(REPL_DIR)/repl.cpp $(LEXER_DIR)/lexer.cpp $(TOKEN_DIR)/token.cpp $(REPL_DIR)/repl_test.cpp -o repl_test.out
	./repl_test.out

ast_test:
	$(CXX) $(CXXFLAGS) $(AST_DIR)/ast.cpp $(TOKEN_DIR)/token.cpp $(AST_DIR)/ast_test.cpp -o ast_test.out
	./ast_test.out

all_tests: ast_test lexer_test repl_test token_test
	@echo "All tests passed!"

clean:
	rm -f *.out