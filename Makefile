
CC      := gcc
CFLAGS  := -Wall -Wextra -g
TEST_CFLAGS := -Wall -Wextra -O2

SHELL_SRCS := c_py_shell.c color_support.c handle_io.c eval.c parser.c lexer.c ast.c sym_table.c

SHELL_OBJS := $(SHELL_SRCS:.c=.o)
SHELL_BIN  := c_shell

LEXER_TEST_SRCS := test_lexer.c lexer.c
LEXER_TEST_BIN  := test_lexer

.PHONY: all
all: $(SHELL_BIN) $(LEXER_TEST_BIN)

$(SHELL_BIN): $(SHELL_OBJS)

	$(CC) $(CFLAGS) $^ -o $@ -lreadline


$(LEXER_TEST_BIN): $(LEXER_TEST_SRCS)
	$(CC) $(TEST_CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean run
clean:
	$(RM) *.o $(SHELL_BIN) $(LEXER_TEST_BIN)

run: $(SHELL_BIN)
	./$(SHELL_BIN)
