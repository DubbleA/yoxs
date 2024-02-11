# YOXS

YOXS is a fully functional interpreter of the Monkey language created by Thorsten Ball written in C++. 

The interpreter utilizes a 3 stage (Lexical Analysis, Abstract-Syntax-Tree Construction & Parsing, and Evaluation) process to translate plaintext into evaluated code.

In addition to the interpreter, there is an API Server with 10+ REST Endpoints connected to a front-end visualizer where users can see the under the hood process of how an interpreter functions. Users can input their own code or pick one of the sampleprograms hosted in MongoDB.

## Progress & Goals

[Link to documentation](ProgressAndGoals.md)

## Front End Repo

[Link to repo](https://github.com/vokpe/yoxs-frontend/tree/main)

## File Structure

The project separates individual components with the structure of:
- .hpp file: Header file defining necessary classes, and forward declarations of methods and attributes. 
- .cpp file: Contains the primary implementation of the package's functionality.
- _test.cpp file: structured tests for the package.

## Testing

run `make tests` to build and run tests and `make clean` to clean the compiled files

# Stage One: Lexing | Lexical Analysis

To convert plaintext into a more usable format we need to convert it from Source Code -> Tokens -> Abstract Syntax Tree. The first transformation, from source code to tokens is called "lexical analysis". 

## Tokens

The primary objective of the tokens package is to define the set of tokens that our lexer will process and produce. These tokens serve as standardized representations of our source code elements.

### Special Tokens
Token Types Analysis

For example tokens contains several token types:
- **Numbers**: Represent integers like 5 and 10.
- **Identifiers**: Variable names like x, y, add, and result.
- **Keywords**: Language-specific words like let and fn.
- **Special Characters**: Symbols including (, ), {, }, =, ,, ;.
We've designated two unique tokens, namely:
- **EOF**: Indicates the end of the file.
- **ILLEGAL**: Represents any unrecognized tokens or symbols.

C++ Implementation:
```cpp
// Enum class for TokenType
enum class TokenType {
    // ... [token types definitions]
};

// Token class
class Token {
    // ... [class definition]
};

// ... [additional functions and operator overloads]
```

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

In our AST we start with three interfaces: Node, Statement, Expression. Every Node in our AST has to implement the Node interface, meaning we have to provide a TokenLiteral() method that returns the literal token value associated with it. TokenLiteral() will be used for debugging and testing. The AST we are constructing will consist of connected nodes, where some will implement the expression or statement interface. They will each contain a statementNode and expressionNode dummy method respectively. They are not strictly necessary but help us in the compilation and possibly causing it to throw errors i.e. when we use a Statement when an Expression. 

This Program node is going to be the root node of every AST our parser produces. Every valid Monkey program is a series of statements. These statements are contained in the Program.Statements, which is just a slice of AST nodes that implement the Statement interface.

## Key Interfaces and Classes

### Node Interface

- **Purpose**: Serves as the base interface for all nodes in the AST.
- **Method**: `TokenLiteral()` - Returns the literal token value of the node. Essential for debugging and testing.

### Statement Interface

- **Purpose**: Implemented by all nodes that are considered statements in the language.
- **Method**: `statementNode()` - A marker method to categorize a node as a Statement.

### Expression Interface

- **Purpose**: Implemented by all nodes that are considered expressions.
- **Method**: `expressionNode()` - A marker method to categorize a node as an Expression.

### Program Class

- **Purpose**: Acts as the root node of every AST. Represents a sequence of statements in a program.
- **Fields**:
  - `Statements`: A collection of statements (AST nodes implementing the Statement interface).

### LetStatement Class

- Represents a 'let' statement in the language.
- Contains the `Token` object for 'let' and nodes for the variable name and value.

### ReturnStatement Class

- Represents a 'return' statement.
- Contains the `Token` object for 'return' and a node for the return value.

### ExpressionStatement Class

- Wraps an expression as a statement.
- Contains the `Token` object and a node representing the expression.

### BlockStatement Class

- Represents a block of statements enclosed in `{}`.
- Contains a collection of statements.

### Identifier Class

- Represents an identifier used in the language.
- Stores the `Token` object and the identifier value.

### Boolean Class

- Represents a boolean literal.
- Stores the `Token` object and the boolean value.

### IntegerLiteral Class

- Represents an integer literal.
- Stores the `Token` object and the integer value.

### PrefixExpression Class

- Represents a prefix expression (e.g., `!true`).
- Contains the operator and the right-hand side expression.

### InfixExpression Class

- Represents an infix expression (e.g., `5 + 5`).
- Contains the left-hand side expression, operator, and right-hand side expression.

### IfExpression Class

- Represents an 'if' conditional expression.
- Contains the condition, consequence, and optional alternative block.

### FunctionLiteral Class

- Represents a function definition.
- Contains parameters and the function body.

### CallExpression Class

- Represents a function call.
- Contains the function identifier or literal and the arguments.

### StringLiteral Class

- Represents a string literal.
- Contains the `Token` object and the string value.

### ArrayLiteral Class

- Represents an array literal.
- Contains a list of elements (expressions).

### IndexExpression Class

- Represents an expression to access an element from an array or a hash.
- Contains the left-hand side expression and the index expression.

### HashLiteral Class

- Represents a hash/map literal.
- Stores pairs of keys and values.

## Utility Functions

### join Function

- Used to concatenate strings with a given delimiter.
- Essential for creating string representations of various nodes.

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

This package defines the objects used to represent different types of values and structures within the programming language, each encapsulating specific data and behaviors.

### ObjectType Enumeration

- **Purpose**: Defines the types of objects that can be represented in the language.
- **Types**: NULL_OBJ, ERROR_OBJ, INTEGER_OBJ, BOOLEAN_OBJ, STRING_OBJ, RETURN_VALUE_OBJ, FUNCTION_OBJ, BUILTIN_OBJ, ARRAY_OBJ, HASH_OBJ.

### Object Class

- **Purpose**: Base class for all objects in the language.
- **Methods**:
  - `Type()`: Returns the object's type.
  - `Inspect()`: Returns a string representation of the object.

### Integer Class

- **Purpose**: Represents an integer value.
- **Fields**: `Value` - The integer value.
- **Methods**: Inherits `Type()` and `Inspect()` from `Object`.

### BooleanObject Class

- **Purpose**: Represents a boolean value.
- **Fields**: `Value` - The boolean value.
- **Methods**: Inherits `Type()` and `Inspect()` from `Object`.

### NullObject Class

- **Purpose**: Represents a null value.
- **Methods**: Inherits `Type()` and `Inspect()` from `Object`.

### ReturnValue Class

- **Purpose**: Represents the return value from a function.
- **Fields**: `Value` - The object returned.
- **Methods**: Inherits `Type()` and `Inspect()` from `Object`.

### Error Class

- **Purpose**: Represents an error.
- **Fields**: `Message` - The error message.
- **Methods**: Inherits `Type()` and `Inspect()` from `Object`.

### Function Class

- **Purpose**: Represents a function.
- **Fields**:
  - `Parameters` - Function parameters.
  - `Env` - The environment in which the function executes.
  - `Body` - The function's body.
- **Methods**: Inherits `Type()` and `Inspect()` from `Object`.

### String Class

- **Purpose**: Represents a string value.
- **Fields**: `Value` - The string value.
- **Methods**: Inherits `Type()` and `Inspect()` from `Object`.

### Builtin Class

- **Purpose**: Represents a built-in function.
- **Fields**: `function` - The function implementation.
- **Methods**: Inherits `Type()` and `Inspect()` from `Object`.

### ArrayObject Class

- **Purpose**: Represents an array.
- **Fields**: `Elements` - The elements in the array.
- **Methods**: Inherits `Type()` and `Inspect()` from `Object`.

### HashPair Class

- **Purpose**: Represents a key-value pair in a hash.
- **Fields**: 
  - `Key` - The key object.
  - `Value` - The value object.

### Hash Class

- **Purpose**: Represents a hash (map/dictionary).
- **Fields**: `Pairs` - A map of hash keys to HashPairs.
- **Methods**: Inherits `Type()` and `Inspect()` from `Object`.

### ObjectTypeToString

- **Purpose**: Converts an `ObjectType` enum value to its string representation.
- **Usage**: Used throughout the package to provide readable object type names.

## Environment

### Purpose

The `Environment` class is designed to store and manage the bindings between variable names and their respective values. It is an essential component for implementing lexical scoping within the programming language.

### Structure

- **Fields**:
  - `outer`: A pointer to an outer environment. This allows the creation of a nested, hierarchical scope structure.
  - `store`: An unordered map that holds the variable bindings, mapping variable names (strings) to their corresponding objects.

### Constructors

- **Environment(std::shared_ptr<Environment> outer = nullptr)**: Constructs an `Environment` instance, optionally linked to an outer environment.

### Methods

- **Get(const std::string& name)**:
  - **Purpose**: Retrieves the object bound to the given variable name.
  - **Parameters**: `name` - The name of the variable.
  - **Returns**: A shared pointer to the `Object` associated with the variable name, or `nullptr` if the variable is not found.

- **Set(const std::string& name, std::shared_ptr<Object> val)**:
  - **Purpose**: Binds a value (object) to a variable name within the environment.
  - **Parameters**: 
    - `name` - The name of the variable.
    - `val` - A shared pointer to the `Object` to bind to the variable name.
  - **Returns**: A shared pointer to the `Object` that was set.

### Usage

The `Environment` class is utilized in the language's runtime to maintain the state of variables. When a variable is referenced, the environment is queried to retrieve its value. When a variable is assigned, the environment is updated with the new binding.

## Evaluator

The `Evaluator` is a crucial component of the programming language's runtime, responsible for processing and interpreting the abstract syntax tree (AST) nodes and executing the program.

### Purpose

The `Evaluator` class is designed to traverse and interpret the AST created by the parser. It evaluates nodes to produce corresponding runtime objects, handling various aspects of language execution like arithmetic operations, control structures, function application, and more.

### Key Methods

- **Eval**: The primary method to evaluate an AST node.
- **evalProgram**: Evaluates a program (sequence of statements).
- **evalBlockStatement**: Evaluates a block of statements.
- **nativeBoolToBooleanObject**: Converts a native boolean to a BooleanObject.
- **evalPrefixExpression**: Evaluates prefix expressions.
- **evalInfixExpression**: Evaluates infix expressions.
- **evalBangOperatorExpression**: Evaluates the '!' operator.
- **evalMinusPrefixOperatorExpression**: Evaluates the '-' operator for prefix expressions.
- **evalIntegerInfixExpression**: Evaluates infix expressions with integer operands.
- **evalStringInfixExpression**: Evaluates infix expressions with string operands.
- **evalIfExpression**: Evaluates 'if' expressions.
- **evalIdentifier**: Evaluates identifiers, resolving their values.
- **isTruthy**: Determines the truthiness of an object.
- **newError**: Creates a new error object.
- **isError**: Checks if an object is an error.
- **evalExpressions**: Evaluates a list of expressions.
- **applyFunction**: Applies a function to its arguments.
- **extendFunctionEnv**: Extends the environment for function execution.
- **unwrapReturnValue**: Extracts the value from a return object.
- **evalIndexExpression**: Evaluates index expressions for arrays and hashes.
- **evalArrayIndexExpression**: Specifically evaluates array index expressions.
- **evalHashLiteral**: Evaluates hash literal expressions.
- **evalHashIndexExpression**: Specifically evaluates hash index expressions.

### Built-in Functions

The `Evaluator` includes a set of built-in functions like `len`, `puts`, `first`, `last`, `rest`, and `push`, each designed to provide fundamental functionalities in the language.

### ObjectConstants Class

- **Purpose**: Provides constant object instances used throughout evaluation.
- **Fields**:
  - `NULL_OBJ`: Represents the null object.
  - `TRUE`: Represents the true BooleanObject.
  - `FALSE`: Represents the false BooleanObject.

### Usage

The `Evaluator` is invoked after parsing the source code into an AST. It recursively evaluates each node of the AST, effectively executing the program. It handles variable bindings, function calls, control structures, and more, translating the static AST into dynamic behaviors defined by the language.
