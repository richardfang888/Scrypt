<!-- # Scrypt - A scripting language coded in C++

(As of Checkpoint 3) This project currently has a built lexer and parser (accepts S expressions), which each accept variables and multiple expressions, a calculator (parser that accepts infix notation), support for the boolean type (complete with operator support), and statements (if, while, and print).

src/calc.cpp: This is a source file that contains the implementation for a parser for infix expressions with its own main function.
src/parse.cpp: This is a source file that contains the implementation for a parser for S expressions with its own main function.
src/lex.cpp: The main function for the lexer.
src/format.cpp: This is a source file that contains the implementation for a formatter which properly formats the inputted program. Contains its own main function.
src/scrypt.cpp: This is a source file that contains the implementation for the interpreter which evaluates a program and outputs what is explicitly printed by print statements. Contains its own main function.
src/lib/lex_functions.cpp: This is a source file that contains the functions for the lexer.
src/lib/statements.cpp: This is a source file that contains the functions for handling statements.
src/lib/format.hpp: This is a header file that contains the declarations of classes and functions related to the formatter.
src/lib/scrypt.hpp: This is a header file that contains the declarations of classes and functions related to the interpreter.
src/lib/statements.hpp: This is a header file that contains the declarations of classes and functions for handling statements.
src/lib/calc.hpp: This is a header file that contains the declarations of classes and functions related to the infix parser.
src/lib/parse.hpp: This is a header file that contains the declarations of classes and functions related to the S expression parser.
src/lib/lex.hpp: This header file contains declarations related to the lexer.
src/lib/token.hpp: This header file contains the struct that has the types of tokens.

MakeFile Usage: 

1) make parse {input S expression}: Compiles and runs parse.cpp
2) make calc {input infix expression}: Compiles and runs calc.cpp
3) make format {input program}: Compiles and runs format.cpp
4) make scrypt {input program}: Compiles and runs scrypt.cpp
