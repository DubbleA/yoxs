# YOXS

YOXS is a C++ interpreter.

## Lexer

The lexer reads in code and transforms it into tokens, represented by the TokenType object. These tokens are data structures that simply store the information regarding the code. They are then in turn parsed and transformed into a syntax tree to be parsed.

## REPL

The REPL, or the read-eval-print-loop, continuously reads in inputs from the user and calls the lexer to parse them.

## Future Plans

Our plans are to implement a an API server via FLASK, though we currently are considering other options as well. Regarding the front-end, we're planning on taking inspiration from this site: https://emu86.pythonanywhere.com/main. Our endpoints are most likely going to be a way to implement front-facing dashboard. We're planning on implementing the database using MongoDB and are considering either AWS or GCP as a cloud provider.
