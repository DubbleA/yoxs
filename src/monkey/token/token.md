# Token Documentation

## Purpose
The primary objective of the tokens package is to define the set of tokens that our lexer will process and produce. These tokens serve as standardized representations of our source code elements.

## Special Tokens
We've designated two unique tokens, namely:
- **EOF**: Indicates the end of the file.
- **ILLEGAL**: Represents any unrecognized tokens or symbols.

## Special Functions

### `std::ostream& operator<<(std::ostream& os, const Token& token);`
This function overloads the `<<` operator, enabling us to directly print our token objects, especially useful during testing.

### `std::string TokenTypeToString(TokenType type);`
Converts a given `TokenType` into its string representation. This aids in debugging and visualization.

### `TokenType LookupIdent(const std::string& ident);`
This function collaborates with the enumerated class to locate the appropriate token. It then creates and returns a `TokenType` object along with the corresponding string literal.