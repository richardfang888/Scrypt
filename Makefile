CC=g++
CFLAGS=-c -std=c++17 -Wall -Wextra -Werror
SRC_DIR=src
LIB_DIR=$(SRC_DIR)/lib

all: parse_output calc_output

parse_output: $(SRC_DIR)/parse.o $(LIB_DIR)/lex_functions.o
	$(CC) $(SRC_DIR)/parse.o $(LIB_DIR)/lex_functions.o -o parse_output

calc_output: $(SRC_DIR)/calc.o $(LIB_DIR)/lex_functions.o
	$(CC) $(SRC_DIR)/calc.o $(LIB_DIR)/lex_functions.o -o calc_output

$(SRC_DIR)/parse.o: $(SRC_DIR)/parse.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/parse.cpp -o $(SRC_DIR)/parse.o

$(SRC_DIR)/calc.o: $(SRC_DIR)/calc.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/calc.cpp -o $(SRC_DIR)/calc.o

$(LIB_DIR)/lex_functions.o: $(LIB_DIR)/lex_functions.cpp
	$(CC) $(CFLAGS) $(LIB_DIR)/lex_functions.cpp -o $(LIB_DIR)/lex_functions.o

clean:
	rm -f parse_output calc_output $(SRC_DIR)/*.o $(LIB_DIR)/*.o

.PHONY: parse
parse: parse_output
	./parse_output

.PHONY: calc
calc: calc_output
	./calc_output