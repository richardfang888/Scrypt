<!-- # Scrypt

src/calc.cpp: This is a source file that contains the implementation for a parser for infix expressions with its own main function.
src/parse.cpp: This is a source file that contains the implementation for a parser for S expressions with its own main function.
src/lex.cpp: The main function for the lexer.
src/lib/lex_functions.cpp: This is a source file that contains the functions for the lexer.
src/lib/calc.hpp: This is a header file that contains the declarations of classes and functions related to the infix parser.
src/lib/parse.hpp: This is a header file that contains the declarations of classes and functions related to the S expression parser.
src/lib/lex.hpp: This header file contains declarations related to the lexer.
src/lib/token.hpp: This header file contains the struct that has the types of tokens.

MakeFile Usage: 

1) make parse {S expression}: Compiles and runs parse.cpp
2) make calc {infix expression}: Compiles and runs calc.cpp
