in our AST we start with three interfaces: Node, Statement, Expression. Every Node in our AST has to implement the Node interface, meaning we have to provide a TokenLiteral() method that returns the literal token value associated with it. TokenLiteral() will be used for debugging and testing. The AST we are constructing will consist of connected nodes, where some will implement the expression or statement interface. They will each contain a statementNode and expressionNode dummy method respectively. They are not strictly necessary but help us in the compilation and possibly causing it to throw errors i.e. when we use a Statement when an Expression. 


This Program node is going to be the root node of every AST our parser produces. Every valid Monkey program is a series of statements. These statements are contained in the Program.Statements, which is just a slice of AST nodes that implement the Statement interface.



To implement the token literal method we need three fields: the identifier, expression, and the token. LetStatement has the fields Name: to hold the identifier and Value for the expression that produces the value. 

With Program, LetStatement and Identifier defined this piece of Monkey source code

let x = 5;
could be represented by an AST looking like this:

*ast.Program | Statements -> *ast.LetStatement | Name | Value 
-> *ast.Identifier
-> *ast.Expression
