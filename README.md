Scrypt - A programming language coded in C++

This project is a programming language coded in C++ which has a lexer, parser, calculator, formatter, and interpreter.

The lexer converts S expressions into separate token types so that they could be evaluated in a way the computer can understand. The lexer handles delimineres, operators (standard, assignment, logical), comparators, numbers, identifiers, booleans, and null values.

    For example, with the input:

        (+(-2 4.444 )
        32(* 5 13.45)(

    The lexer converts it into the following where the columns are line number, column number, and token:

        1    1  (
        1    2  +
        1    3  (
        1    4  -
        1    5  2
        1    7  4.444
        1   13  )
        2    1  32
        2    3  (
        2    4  *
        2    6  5
        2    8  13.45
        2   13  )
        2   14  (
        3    1  END

The parser uses the tokens that the lexer created to create an abstract syntax tree, which is then used to print an inputted S expression in a readable (infix) form and prints the result after evaluation. It has support for addition, subtraction, multiplication, parentheses, variables, and can read multiple lines at a time.

    For example, with the input:

        (= a b 3)
        (- (= b (+ b 5)) 7)
        (* a b)

    The parser outputs the following in infix and evaluated:

        (a = b = 3)
        3
        ((b = (b + 5)) - 7)
        1
        (a * b)
        24

The calculator is a heavily altered version of the parser that accepts infix notation instead of S expressions, and supports many more types of operations/data. In addition to the basic operations that the normal parser supports (see parser description), calculator supports everything the lexer can handle (assignment, comparisons, logical (and + XOR + inclusive + operators)), new data types like booleans, arrays, values such as null, statements (if, while, print), etc.

    For example, with the input:

        array = [true, 2, 1+1+1, 4, [5]];
        print array[2];
        print array;

        arref = array;
        temp  = arref[1];
        arref[1] = 0 - arref[3];
        arref[3] = 0 - temp;
        print array;

    It will be evaluated into the ouput:

        3
        [true, 2, 3, 4, [5]]
        [true, -4, 3, -2, [5]]

The formatter properly formats the inputted program. It has support for functions in addition to everything calculator supports (see calculator description above).

    For example, with the input:

        z = 42;

        def foo(x, y) {
        def square(value) {
            return value * value;
        }

        print square(x + y + z);
        }

        z = 108;
        f = foo;

        result = f(1, 2);
        if result != null {
        print result;
        }

    It will be formatted into the output:

        (z = 42);
        def foo(x, y) {
            def square(value) {
                return (value * value);
            }
            print square(((x + y) + z));
        }
        (z = 108);
        (f = foo);
        (result = f(1, 2));
        if (result != null) {
            print result;
        }

The interpreter (scrypt) evaluates the inputted program and outputs what it is told to print by the print statements in the program. It has support for everything the formatter supports (see formatter description above).

    For example, using the same input as the formatter example above, the evaluated output would be:

        2025

Specific File Descriptions:

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

1. make parse {input S expression}: Compiles and runs parse.cpp. Takes an S expression as standard input, and if it is valid, outputs the expression in infix form as well as the result of it.

2. make calc {input infix expression}: Compiles and runs calc.cpp. Takes an infix expression as standard input, and if it is valid, outputs the infix form (with parentheses) as well as the result of it.

3. make format {input program}: Compiles and runs format.cpp. Takes a program as standard input, and if it is valid, outputs the program as a formatted version.

4. make scrypt {input program}: Compiles and runs scrypt.cpp. Takes a program as standard input, and if it is valid, outputs what the program explicitly wants to print.

5. make clean: Gets rid of all junk files such as object files and output files.