CC = gcc
CFLAGS = -Wall -Wextra -Werror 

all: c_shell

c_shell: c_py_shell.c
	$(CC) $(CFLAGS) $< -o $@
