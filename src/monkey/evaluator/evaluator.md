Static Member Variables:
A static member variable is shared across all instances of the class, rather than each instance having its own separate copy.
Static member variables have class scope, which means they can be accessed even without creating an instance of the class.
They are initialized only once, at the start of the program execution, and retain their values between function calls.

Static Member Functions:
Static member functions can be called without an instance of the class.
They can access static data members and other static member functions directly.
They cannot access non-static data members or member functions.

making the Eval method static allows it to be called without needing to create an instance of the Evaluator class. This use of static provides a way to organize our code and encapsulate related functionality together


This setup has several advantages:

Type Safety: The compiler will enforce that you handle all node types, and you'll get a compile-time error if you forget to handle a type.
Clarity: The visitor struct separates the evaluation logic for each node type into distinct functions, which can make the code easier to understand and maintain.
Performance: std::visit can be more performant than a series of if-else statements with dynamic_cast, especially if the set of possible types is large.