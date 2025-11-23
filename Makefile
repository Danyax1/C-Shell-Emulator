CC := gcc
# use an explicit relative include path so builds from WSL/Git-Bash find headers
CFLAGS := -Wall -Wextra -O2 -I./include
TEST_CFLAGS := $(CFLAGS)
LDFLAGS :=

SRCDIR := src
TESTDIR := test
OBJDIR := obj
BINDIR := bin

SHELL_SRCS := c_py_shell.c color_support.c handle_io.c eval.c parser.c lexer.c ast.c sym_table.c
SHELL_SRCS := $(addprefix $(SRCDIR)/,$(SHELL_SRCS))
SHELL_OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SHELL_SRCS))
SHELL_BIN := $(BINDIR)/c_shell

LEXER_TEST_BIN := $(BINDIR)/test_lexer
LEXER_TEST_SRCS := $(TESTDIR)/test_lexer.c $(SRCDIR)/lexer.c

.PHONY: all clean run test help
all: $(SHELL_BIN) $(LEXER_TEST_BIN)

$(SHELL_BIN): $(SHELL_OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) $(CFLAGS) $^ -o $@ -lreadline

$(LEXER_TEST_BIN): $(LEXER_TEST_SRCS) | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -Iinclude $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR) $(BINDIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BINDIR) *.o $(notdir $(SHELL_BIN)) $(notdir $(LEXER_TEST_BIN))

.PHONY: run
run: $(SHELL_BIN)
	./$(SHELL_BIN)

.PHONY: test
test: $(LEXER_TEST_BIN)
	@echo "Run $(LEXER_TEST_BIN) to execute lexer tests"

.PHONY: help
help:
	@echo "Usage: make [target]"
	@echo "Targets:" 
	@echo "  all (default) - build shell and tests"
	@echo "  clean         - remove build artifacts"
	@echo "  run           - run the shell binary"
	@echo "  test          - build test binaries" 

CC      := gcc
CFLAGS  := -Wall -Wextra -O2
