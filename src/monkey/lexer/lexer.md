# Lexer Documentation

## Purpose
The lexer's main objective is to transform our source code into a structured format that facilitates subsequent processing. This process, termed "lexical analysis," involves converting plaintext source code into a series of "tokens".

## Example Transformation
Given the input:

```bash
let x = 5 + 5;
```
The lexer produces the following sequence of tokens:
```vbnet
Copy code
[
  LET,
  IDENTIFIER("x"),
  EQUAL_SIGN,
  INTEGER(5),
  PLUS_SIGN,
  INTEGER(5),
  SEMICOLON
]
```

## File Overview
lexer_test.cpp: Provides a testing framework for our lexer. It feeds pre-defined input strings to the lexer, expecting specific tokens as outputs.

Compilation and execution:
```bash
g++ -std=c++17 -Isrc -o lexer_test src/monkey/lexer/lexer.cpp src/monkey/token/token.cpp src/monkey/lexer/lexer_test.cpp && ./lexer_test
```
lexer.cpp: Contains the primary implementation of the lexer functionality.

lexer.hpp: This header file defines the Lexer class, offering necessary methods and attributes for performing lexical analysis.

## Process
We will take our source code as input and call NextToken(), which will output the next token character by character until we reach the end of the file. 

We are going to accomplish this by using a two pointer approach, where we extract tokens character by character using our peekChar and readChar helper functions. 

Finally creating the respective token objects that we have already defined in the token package. 