#include "handle_io.h"
#include "eval.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

int ends_with_colon(const char *s)
{
    if (!s) return 0;
    int len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        len--;

    return (len > 0 && s[len - 1] == ':');
}

int is_empty_line(const char *s)
{
    if (!s) return 1;
    for (; *s; s++)
        if (*s != ' ' && *s != '\t' && *s != '\n')
            return 0;
    return 1;
}

IOStatus handle_io(const char *input)
{
    if (!input) return IO_OK;

    if (strcmp(input, "quit") == 0)
        return IO_EXIT;

    if (strcmp(input, "help") == 0) {
        printf("Available commands: help, quit\n");
        return IO_OK;
    }

    eval(input);
    return IO_OK;
}
