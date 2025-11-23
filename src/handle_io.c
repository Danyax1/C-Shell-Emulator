#include "handle_io.h"
#include "eval.h"
#include "sym_table.h"
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

    if (strncmp(input, "quit\n", 5) == 0 || strncmp(input, "quit()\n", 7) == 0){
        free_eval();
        return IO_EXIT;
    }

    if (strncmp(input, "help\n", 5) == 0 || strncmp(input, "help()\n", 7) == 0) {
        printf("Welcome to Python shell!\n");
        printf("You can use it for ariphmetic and locigal operations\n");
        printf("Shell supports integers, floats, True and False\n");
        printf("For flow control shell provides if/elif/else and while statements\n");
        printf("For further info refer to the DOCUMENTATION.md\n");
        return IO_OK;
    }

    if (is_empty_line(input)) return IO_OK;

    // Evaluate input
    Value result = eval(input);

    // Only print if it's not a "statement result"
    // (like assignment or void result)
    if (result.valueType != VAL_NONE) {
        switch (result.valueType)
        {
        case VAL_INT:
            printf("%lld\n", result.value.IntVal);
            break;
        case VAL_FLOAT:
            printf("%lf\n", result.value.FloatVal);
            break;
        case VAL_BOOL:
            if(result.value.BoolVal == 1) {printf("True\n");}
            else{printf("False\n");}
            break;
        
        default:
            break;
        }
    }

    return IO_OK;
}
