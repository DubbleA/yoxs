# YOXS

YOXS is a fully functional interpreter of the Monkey language created by Thorsten Ball written in C++. 

YOXS utilizes a 3 stage (Lexical Analysis, Abstract-Syntax-Tree Construction & Parsing, and Evaluation) process to translate plaintext into evaluated code.

## File Structure

The project separates individual components with the structure of:
- .hpp file: Header file defining necessary classes, and forward declarations of methods and attributes. 
- .cpp file: Contains the primary implementation of the package's functionality.
- _test.cpp file: structured tests for the package.

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

## REPL | Stage 1

The REPL, or the read-eval-print-loop, continuously reads in inputs from the user and calls the lexer to parse them. Similar to what's found in languages like Python and Ruby, our REPL reads input, sends it to the interpreter for processing, and then loops back to await further input.

### Reading & Tokenization
At this stage, our REPL primarily focuses on reading input and processing it into its tokenized form. While the term "Evaluate" is part of the REPL acronym, we aren't evaluating our source code yet. The current emphasis lies on capturing the user's input and transforming it into a series of tokens that provide a structured representation of the code.

### Continuation Until EOF
Our REPL continues to read new input iteratively. This loop persists until the system encounters the designated "end of file" (EOF) token, signaling the REPL to terminate.

### Note
The tokens produced by the REPL are integral to the process, serving as the bridge between raw user input and the interpreter. For more details on the tokenization process and the structure of tokens, please refer to the **Token Documentation**.

## Example Function
let x = 10;
let y = 15;

let add = fn(a, b) {
  return a + b;
};

## Future Plans

Our plans are to implement a an API server via FLASK, though we currently are considering other options as well. Regarding the front-end, we're planning on taking inspiration from this site: https://emu86.pythonanywhere.com. Our endpoints are most likely going to be a way to implement front-facing dashboard. We're planning on implementing the database using MongoDB and are considering either AWS or GCP as a cloud provider.

# Stage Two: Parsing 

## Abstract Syntax Tree (AST)

## Parser (Recursive Descent Pratt Parsing)

# Stage Three: Evaluation

## Object

## Environment

## Evaluator