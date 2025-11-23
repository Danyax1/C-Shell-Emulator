CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2 -I./include
CXXFLAGS = -Wall -Wextra -O2 -std=c++17 -I./include
LDLIBS = -lreadline
GTEST_LIBS = -lgtest -lgtest_main -pthread

SHELL_C_FILES = src/c_py_shell.c src/color_support.c src/handle_io.c src/eval.c src/parser.c src/lexer.c src/ast.c src/sym_table.c

SHELL_OBJ_FILES = obj/color_support.o obj/handle_io.o obj/eval.o obj/parser.o obj/lexer.o obj/ast.o obj/sym_table.o

MAIN_OBJ = obj/c_py_shell.o

TEST_LEXER_CPP = test/test_lexer.cpp
TEST_SYM_TABLE_CPP = test/test_sym_table.cpp

TEST_LEXER_BIN = bin/test_lexer
TEST_SYM_TABLE_BIN = bin/test_sym_table

SHELL_BIN = bin/py_shell

.PHONY: all clean run test help

all: $(SHELL_BIN)

$(SHELL_BIN): $(SHELL_OBJ_FILES) $(MAIN_OBJ) | bin
	$(CC) $(CFLAGS) $(SHELL_OBJ_FILES) $(MAIN_OBJ) -o $(SHELL_BIN) $(LDLIBS)

$(TEST_LEXER_BIN): $(TEST_LEXER_CPP) $(SHELL_OBJ_FILES) | bin
	$(CXX) $(CXXFLAGS) $(TEST_LEXER_CPP) $(SHELL_OBJ_FILES) -o $(TEST_LEXER_BIN) $(GTEST_LIBS) $(LDLIBS)

$(TEST_SYM_TABLE_BIN): $(TEST_SYM_TABLE_CPP) $(SHELL_OBJ_FILES) | bin
	$(CXX) $(CXXFLAGS) $(TEST_SYM_TABLE_CPP) $(SHELL_OBJ_FILES) -o $(TEST_SYM_TABLE_BIN) $(GTEST_LIBS) $(LDLIBS)

obj/c_py_shell.o: src/c_py_shell.c | obj
	$(CC) $(CFLAGS) -c src/c_py_shell.c -o obj/c_py_shell.o

obj/color_support.o: src/color_support.c | obj
	$(CC) $(CFLAGS) -c src/color_support.c -o obj/color_support.o

obj/handle_io.o: src/handle_io.c | obj
	$(CC) $(CFLAGS) -c src/handle_io.c -o obj/handle_io.o

obj/eval.o: src/eval.c | obj
	$(CC) $(CFLAGS) -c src/eval.c -o obj/eval.o

obj/parser.o: src/parser.c | obj
	$(CC) $(CFLAGS) -c src/parser.c -o obj/parser.o

obj/lexer.o: src/lexer.c | obj
	$(CC) $(CFLAGS) -c src/lexer.c -o obj/lexer.o

obj/ast.o: src/ast.c | obj
	$(CC) $(CFLAGS) -c src/ast.c -o obj/ast.o

obj/sym_table.o: src/sym_table.c | obj
	$(CC) $(CFLAGS) -c src/sym_table.c -o obj/sym_table.o

obj:
	mkdir -p obj

bin:
	mkdir -p bin

clean:
	rm -rf obj bin

run: $(SHELL_BIN)
	./$(SHELL_BIN)

test: $(TEST_LEXER_BIN) $(TEST_SYM_TABLE_BIN)
	@echo "Running test_lexer..."
	./$(TEST_LEXER_BIN)
	@echo ""
	@echo "Running test_sym_table..."
	./$(TEST_SYM_TABLE_BIN)
	@echo ""
	@echo "All tests passed."

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all (default) - build shell binary"
	@echo "  test          - build and run all tests"
	@echo "  run           - run shell"
	@echo "  clean         - remove build artifacts"