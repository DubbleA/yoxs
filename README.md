# YOXS

YOXS is a fully functional interpreter of the Monkey language created by Thorsten Ball written in C++. 

YOXS utilizes a 3 stage (Lexical Analysis, Abstract-Syntax-Tree Construction & Parsing, and Evaluation) process to translate plaintext into evaluated code.

## File Structure

The project separates individual components with the structure of: \\
.hpp file: Header file defining necessary classes, and forward declarations of methods and attributes \\
.cpp file: Contains the primary implementation of the package's functionality \\
_test.cpp file: structured tests for the package

## Testing

run `make tests` to build and run tests and `make clean` to clean the compiled files

# Stage One: Lexing | Lexical Analysis

## Tokens

The primary objective of the tokens package is to define the set of tokens that our lexer will process and produce. These tokens serve as standardized representations of our source code elements.

### Special Tokens
We've designated two unique tokens, namely:
- **EOF**: Indicates the end of the file.
- **ILLEGAL**: Represents any unrecognized tokens or symbols.

### Special Functions

```cpp
std::ostream& operator<<(std::ostream& os, const Token& token);
std::string TokenTypeToString(TokenType type);
```
These functions are used to overload the `<<` operator, enabling us to directly print our token objects. This aids in debugging and visualization.

```cpp
TokenType LookupIdent(const std::string& ident);
```

This function collaborates with the enumerated class to locate the appropriate token. It then creates and returns a `TokenType` object along with the corresponding string literal.

## Lexer

The lexer's main objective is to transform our source code into a structured format that facilitates subsequent processing. This process, termed "lexical analysis," involves converting plaintext source code into a series of "tokens" represented by the TokenType object. 

In simple terms, the lexer reads in code and transforms it into tokens which are simply data structures that store the information regarding the code. Eventaully they will be parsed and transformed into a syntax tree in Stage Two.

### Example Transformation
Given the input:

```js
let x = 5 + 5;
```

The lexer produces the following sequence of tokens:

```js
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

## Start of a REPL

The REPL, or the read-eval-print-loop, continuously reads in inputs from the user and calls the lexer to parse them.

## Future Plans

Our plans are to implement a an API server via FLASK, though we currently are considering other options as well. Regarding the front-end, we're planning on taking inspiration from this site: https://emu86.pythonanywhere.com. Our endpoints are most likely going to be a way to implement front-facing dashboard. We're planning on implementing the database using MongoDB and are considering either AWS or GCP as a cloud provider.
