# C-Shell-Emulator

A small C project implementing a minimal Python-like interactive shell. It includes a lexer, parser (Pratt-style), AST, evaluator, a simple symbol table, and a readline-based interactive frontend.

This document explains how to build, run and test the project and describes the key source files.

**Repository layout**

- `c_py_shell.c` — program entrypoint and interactive loop (uses `readline` for input and history).
- `color_support.c`, `colors.h` — console color helpers (attempts to enable ANSI sequences on Windows).
- `handle_io.c`, `handle_io.h` — top-level input handling and simple REPL commands (`help`, `quit`).
- `lexer.c`, `lexer.h` — tokenizer for the shell's language (numbers, identifiers, keywords, operators, and indentation tokens).
- `parser.c`, `parser.h` — Pratt parser that builds an AST from the token stream.
- `ast.c`, `ast.h` — AST node constructors, block helpers, and pretty-printing utilities.
- `eval.c`, `eval.h` — evaluator that executes AST nodes and uses `sym_table` for variables.
- `sym_table.c`, `sym_table.h` — simple hash-table-backed symbol table with linear probing and tombstones.
- `Makefile` — build rules for the shell and the lexer tests.
- `test_lexer.c`, `test_sym_table.c` — small manual tests used during development.
- `README.md` — short project intro and contributors.

**Features**

- Interactive prompt with history support (via `readline`).
- Python-like language features: variable assignment, numeric and boolean literals, arithmetic, comparisons, boolean operators, `if`/`elif`/`else`, `while`, and `print`.
- Indentation-aware lexer producing `TOKEN_INDENT` / `TOKEN_DEDENT` tokens to represent block structure.
- Small evaluator and symbol table for integers, floats and booleans.

Prerequisites

- A C compiler (the `Makefile` uses `gcc`).
- `make` (recommended) or the ability to run the `gcc` commands manually.
- `readline` development library for full line-editing and history support. On Windows, `readline` is typically not available by default — use MSYS2/MinGW or WSL.

Building

From the project root you can build both the interactive shell and the lexer test with `make`:

```powershell
make all
```

If you need to compile manually, example commands (require `gcc` and `readline`):

```powershell
gcc -Wall -Wextra -g c_py_shell.c color_support.c handle_io.c eval.c parser.c lexer.c ast.c sym_table.c -o c_shell -lreadline
gcc -Wall -Wextra -O2 test_lexer.c lexer.c -o test_lexer
```

Notes for Windows

- Building natively on Windows is easier with MSYS2/MinGW or WSL. The `readline` library may not be present in typical Visual Studio toolchains. If `-lreadline` causes link errors, either install a compatible `readline` port or remove the `-lreadline` flag from the `Makefile` (you'll lose improved line editing and history support).
- ANSI color support is attempted on Windows; if it can't be enabled, the shell will still run without colored output.

Running the shell

- Start the interactive shell:

```powershell
./c_shell
```

- Inside the shell you can run expressions and small blocks using indentation:

```
>>> x = 10
>>> print(x + 2)
12
>>> if x > 5:
...     print(True)
...
True
```

Testing

- Run the lexer tests built by the `Makefile`:

```powershell
./test_lexer
```

- Compile and run the symbol-table test manually:

```powershell
gcc -Wall -Wextra -O2 test_sym_table.c sym_table.c -o test_sym_table
./test_sym_table
```

Troubleshooting & Known Issues

- `readline` missing: If the build fails due to linking `-lreadline`, install `readline` for your environment or remove the `-lreadline` flag from the `Makefile`.
- Windows compilation: Use MSYS2/MinGW or WSL for an easier experience. Native Visual Studio builds require more changes.
- Incomplete or under-development features: The codebase contains TODOs and some error handling is minimal. Expect that parser/evaluator error reporting can be improved.
- Large keys in `sym_table`: Very long keys will increase memory usage; the symbol table stores keys using `strdup`.
- Float and int values are limited

Contributing

- Contributions are welcome. Suggested improvements: expand tests, improve error handling and messages, add CI, make the Windows build smoother, and add more builtins.

Possible follow-ups I can do for you

- Add a `make test` target to the `Makefile` that builds and runs available tests.
- Add a `BUILD.md` with step-by-step platform-specific build instructions.
- Improve the `Makefile` to build `test_sym_table` automatically.

If you'd like any of those follow-ups, tell me which one to do next.
