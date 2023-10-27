Wikipedia: A parser is a software component that takes input data (frequently text) and builds a data structure – often some kind of parse tree, abstract syntax tree or other hierarchical structure – giving a structural representation of the input, checking for correct syntax in the process. […] The parser is often preceded by a separate lexical analyser, which creates tokens from the sequence of input characters

The “abstract” is based on the fact that certain details visible in the source code are omitted in the AST.

They take source code as input (either as text or tokens) and produce a data structure which represents this source code. While building up the data structure, they unavoidably analyse the input, checking that it conforms to the expected structure. Thus the process of parsing is also called syntactic analysis.

Its input will be the tokens we defined in the previous chapter, produced by the lexer we already wrote. We will define our own AST, suited to our needs as interpreters of the Monkey programming language, and construct instances of this AST while recursively parsing tokens.

There are a lot of parser generators, differing in the format of the input they accept and the language of the output they produce. The majority of them use a context-free grammar (CFG) as their input. A CFG is a set of rules that describe how to form correct (valid according to the syntax) sentences in a language. The most common notational formats of CFGs are the Backus-Naur Form (BNF) or the Extended Backus-Naur Form (EBNF).

The parser we are going to write is a recursive descent parser. And in particular, it’s a “top down operator precedence” parser, sometimes called “Pratt parser”, after its inventor Vaughan Pratt.

Here is a fully valid program written in Monkey:

let x = 10;
let y = 15;

let add = fn(a, b) {
  return a + b;
};

Programs in Monkey are a series of statements. In this example we can see three statements, three variable bindings - let statements - of the following form:

let <identifier> = <expression>;
A let statement in Monkey consists of two changing parts: an identifier and an expression. In the example above x, y and add are identifiers. 10, 15 and the function literal are expressions.



10/16 
going to make the design decision to change:
typedef Expression*(Parser::*prefixParseFn)();
typedef Expression*(Parser::*infixParseFn)(Expression*);

to 

using prefixParseFn = std::function<std::unique_ptr<Expression>(void)>;
using infixParseFn = std::function<std::unique_ptr<Expression>(std::unique_ptr<Expression>)>;

This is because I have gotten to the point in Scott Meyers Effective modern c++ book where he encourages "using alias declarations" over typedef for creating aliases. 

To briefly explain, my interpretation of his reasoning is the fact that 'using' syntax is more readable, and is consistent with template type aliasing.

I am not entirely sure tbh, just kinda winging it if im being honest. 


The Program node is going to be the root node of every AST our parser produces. Every valid Monkey program is a series of statements. These statements are contained in the Program.Statements, which is just a slice of AST nodes that implement the Statement interface.

file:///Users/nlmb/Documents/writing_an_interpreter_in_go_1.7/images/let_statement.png