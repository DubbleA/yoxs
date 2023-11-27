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

### Introduction
The Pratt Parser, a top-down operator precedence parsing algorithm, is implemented here. It differs from context-free grammar parsers by associating parsing functions with token types, enhancing efficiency in parsing expressions. 

### The Core Idea:
In Pratt parsing, each token type can have up to two parsing functions associated with it, depending on its position in an expression—prefix or infix. The `prefixParseFns` and `infixParseFns` maps in our parser hold these parsing functions. 

### How Pratt Parsing Works

The parseExpression method is pivotal, combining parsing functions and precedence levels to parse expressions. This ensures correct nesting in the Abstract Syntax Tree (AST).

Parsing Example: `1 + 2 + 3;`
The expression `1 + 2 + 3;` is parsed to yield an AST resembling `((1 + 2) + 3)`. The process involves multiple steps, starting with parseExpressionStatement and proceeding through parsing individual integers and operators to construct the AST.

#### Step-by-Step Parsing:

1. **Initial Call**:
   - `parseExpressionStatement` is called, which in turn calls `parseExpression(LOWEST)` with the current and peek tokens being `1` and `+`, respectively.

2. **Parsing `1`**:
   - `parseExpression` finds a `prefixParseFn` for `token.INT`, which is `parseIntegerLiteral`, returning an `ast.IntegerLiteral` node representing `1`.
   - This node is assigned to `leftExp`.

3. **Entering the Loop**:
   - A loop in `parseExpression` checks if the precedence of the peek token (`+`) is higher than the current precedence level (`LOWEST`). It is, so it proceeds.

4. **Parsing First `+`**:
   - The `infixParseFn` for `token.PLUS`, which is `parseInfixExpression`, is retrieved and called, passing `leftExp` (representing `1`) as an argument.
   - Inside `parseInfixExpression`, a new `ast.InfixExpression` node is created with `leftExp` as its left child. 
   - `parseExpression` is then recursively called with the precedence level of `+`, to parse the right-hand side of the expression. 

5. **Parsing `2`**:
   - `parseExpression` is called again, this time parsing `2` and returning an `ast.IntegerLiteral` node representing `2`, which becomes the right child of the `ast.InfixExpression` node.

6. **Back to Outer Loop**:
   - Control returns to the outermost loop in `parseExpression`. 
   - The condition of the loop is evaluated again, and since the precedence of the next `+` is equal to the current precedence level, it exits the loop and returns the `ast.InfixExpression` node representing `(1 + 2)`.

7. **Parsing Second `+`**:
   - The loop in `parseExpression` is entered again, this time with `leftExp` representing `(1 + 2)`.
   - The `infixParseFn` for `token.PLUS` is called again, passing `leftExp` as an argument.

8. **Parsing `3`**:
   - Inside `parseInfixExpression`, `parseExpression` is called again to parse `3`, returning an `ast.IntegerLiteral` node representing `3`.
   - This node becomes the right child of a new `ast.InfixExpression` node, with `leftExp` (representing `(1 + 2)`) as its left child.

9. **Final AST**:
   - The final AST representing `((1 + 2) + 3)` is returned.

### Implementation in C++

In this C++ implementation, the parser is structured within a `Parser` class, encapsulating the parsing logic and state. The core parsing methods, `parseExpression`, `parseInfixExpression`, and `parseIntegerLiteral`, among others, implement the Pratt parsing algorithm as described above.

### Parsing Functions: Prefix and Infix
The parsing functions are central to the Pratt parsing technique. They are associated with token types and are stored in `prefixParseFns` and `infixParseFns` maps.

- **Prefix Parsing Functions (`nuds` in Pratt terminology)**:
  These functions parse the "value" of a prefix expression or a standalone token.

```c++
using prefixParseFn = std::function<std::shared_ptr<Expression>(void)>;
std::unordered_map<TokenType, prefixParseFn> prefixParseFns;
```

### Precedence Levels
Precedence levels help determine the order of operations. They are defined in an enum and used to decide when to "reduce" a series of tokens to an expression.

```c++
enum Precedence {
    LOWEST = 1,
    EQUALS,  // ==
    LESSGREATER,  // > or <
    SUM,  // +
    PRODUCT,  // *
    PREFIX,  // -X or !X
    CALL  // myFunction(X)
};
```

### Parsing Methods
Various parsing methods are defined on the `Parser` class to handle different aspects of the syntax. These methods work together, recursively calling each other to parse and build the AST.

```c++
std::shared_ptr<Expression> parseExpression(Precedence precedence);
std::shared_ptr<InfixExpression> parseInfixExpression(std::shared_ptr<Expression> left);
std::shared_ptr<IntegerLiteral> parseIntegerLiteral();
// ... other parsing methods
```

Each parsing method corresponds to a specific grammar rule or a part of a rule in the Monkey language specification. For instance, `parseExpression` orchestrates the parsing of expressions based on precedence levels, while `parseInfixExpression` and `parseIntegerLiteral` handle infix expressions and integer literals, respectively.

### Error Handling
The parser includes error handling to provide meaningful feedback when encountering syntax errors. The `errors` vector collects error messages during the parsing process.

```c++
std::vector<std::string> errors;
```

Methods like `peekError` and `noPrefixParseFnError` are utilized to generate error messages when unexpected tokens are encountered or when no parsing function is found for a given token type, respectively. These methods help in identifying and reporting errors in a user-friendly manner, aiding in debugging and correcting the Monkey code.

```c++
void peekError(TokenType t);
void noPrefixParseFnError(TokenType t);
```

### Token Advancement
Token advancement is a crucial part of the parsing process. The `nextToken` method is used to advance the `curToken` and `peekToken` to the next tokens in the input stream, facilitating the parsing functions to examine and consume tokens as needed.

```c++
void nextToken();
```

The `curToken` and `peekToken` act like two "pointers" analogous to the position and readPosition in our lexer, but instead of pointing to a character in the input, they point to the current and the next token, respectively. Both are crucial as we need to look at the `curToken`, which is the current token under examination, to decide what to do next, and we also need `peekToken` for this decision if `curToken` doesn’t provide enough information.

```c++
Token curToken;
Token peekToken;
```

### Assertion Functions
Assertion functions like `curTokenIs`, `peekTokenIs`, and `expectPeek` are utilized to enforce the correct order of tokens by checking the type of the current or next token. They assist in ensuring that the correct syntax is followed, and if not, they contribute to error reporting.

# Stage Three: Evaluation

## Object

## Environment

## Evaluator