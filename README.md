# YOXS

YOXS is a fully functional interpreter of the Monkey language created by Thorsten Ball written in C++. 

YOXS utilizes a 3 stage (Lexical Analysis, Abstract-Syntax-Tree Construction & Parsing, and Evaluation) process to translate plaintext into evaluated code.

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

The lexer reads in code and transforms it into tokens, represented by the TokenType object. These tokens are data structures that simply store the information regarding the code. They are then in turn parsed and transformed into a syntax tree to be parsed.

## Start of a REPL

The REPL, or the read-eval-print-loop, continuously reads in inputs from the user and calls the lexer to parse them.

## Future Plans

Our plans are to implement a an API server via FLASK, though we currently are considering other options as well. Regarding the front-end, we're planning on taking inspiration from this site: https://emu86.pythonanywhere.com. Our endpoints are most likely going to be a way to implement front-facing dashboard. We're planning on implementing the database using MongoDB and are considering either AWS or GCP as a cloud provider.
