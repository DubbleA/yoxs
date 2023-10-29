# REPL Documentation

## Introduction
The REPL, or Read-Evaluate-Print Loop, is a simple, interactive programming environment. Similar to what's found in languages like Python and Ruby, our REPL reads input, sends it to the interpreter for processing, and then loops back to await further input.

## Current Functionality
.....

# Pratt Parser Documentation for Monkey Language

## Introduction
The Pratt Parser implemented in this project follows a top-down operator precedence parsing algorithm. Unlike context-free grammar parsers, Pratt Parser associates parsing functions with token types, which allows for a more straightforward and efficient parsing of expressions. 

## How Pratt Parsing Works
The Pratt parsing technique hinges on the `parseExpression` method, combining parsing functions and precedence levels to parse expressions correctly. This mechanism allows for parsing expressions and ensuring the correct nesting of operators and operands within the Abstract Syntax Tree (AST).

### Parsing Example: `1 + 2 + 3;`
Consider parsing the expression `1 + 2 + 3;`. The goal is to correctly represent the operator precedence and associativity, yielding an AST resembling `((1 + 2) + 3)`.

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

## Implementation in C++

In this C++ implementation, the parser is structured within a `Parser` class, encapsulating the parsing logic and state. The core parsing methods, `parseExpression`, `parseInfixExpression`, and `parseIntegerLiteral`, among others, implement the Pratt parsing algorithm as described above.

### Parsing Functions: Prefix and Infix
The parsing functions are central to the Pratt parsing technique. They are associated with token types and are stored in `prefixParseFns` and `infixParseFns` maps.

- **Prefix Parsing Functions (`nuds` in Pratt terminology)**:
  These functions parse the "value" of a prefix expression or a standalone token.

```c++
using prefixParseFn = std::function<std::unique_ptr<Expression>(void)>;
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
std::unique_ptr<Expression> parseExpression(Precedence precedence);
std::unique_ptr<InfixExpression> parseInfixExpression(std::unique_ptr<Expression> left);
std::unique_ptr<IntegerLiteral> parseIntegerLiteral();
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

```c++
bool curTokenIs(TokenType t);
bool peekTokenIs(TokenType t);
bool expectPeek(TokenType t);
```

### Example:

```c++
if (expectPeek(TokenType::PLUS)) {
    // ... proceed with parsing an addition expression
} else {
    // ... report an error as a '+' was expected
}
```

In the example above, `expectPeek` is used to assert that the next token in the stream is a `+`. If this assertion holds, the parsing of an addition expression is carried out. Otherwise, an error is reported indicating that a `+` was expected, aiding in pinpointing syntax errors for correction.

This mechanism of asserting the expected tokens and branching the parsing logic accordingly is a cornerstone of the Pratt parser's design. It ensures that the parser reacts correctly to the structure of the code, guiding the construction of the AST based on the tokens it encounters. This level of control and feedback is crucial for building a reliable and user-friendly parser.

Through a combination of parsing functions, precedence levels, and assertion functions, the Pratt parser effectively handles the complexities of the Monkey language's syntax, providing a solid foundation for the interpreter to evaluate the code. With a structured approach to error handling, the parser also provides meaningful feedback to the user, facilitating debugging and code correction.

This approach showcases a meticulous and organized parsing process, demonstrating the Pratt parser's adaptability and efficiency in parsing a variety of expressions in the Monkey language. Through the design of the `Parser` class and its associated methods, the parsing process is both extendable and maintainable, paving the way for further language features and enhancements.

### Pratt Parsing: A Deeper Dive
The Pratt parsing technique, inspired by Vaughan Pratt's work, is a top-down operator precedence parsing method that associates parsing functions with token types. This approach is distinguished from traditional parsing methods that often relate parsing functions with grammar rules.

#### The Core Idea:
In Pratt parsing, each token type can have up to two parsing functions associated with it, depending on its position in an expression—prefix or infix. The `prefixParseFns` and `infixParseFns` maps in our parser hold these parsing functions. 