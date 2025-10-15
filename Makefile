CC = gcc
CFLAGS = -Wall -Wextra -Werror 

all: c_shell lexer test_lexer

c_shell: c_py_shell.c
	$(CC) $(CFLAGS) $< -o $@

lexer: lexer.c
	$(CC) $(CFLAGS) lexer.c $< -o lexer.o

test: test_lexer.c
	$(CC) $(CFLAGS) lexer.c $< -o $@
