#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"
#include <stdarg.h>
#include <readline/readline.h>
#include <readline/history.h>

// static void print_error(const char *fmt, ...) {
//     va_list ap;
//     va_start(ap, fmt);
//     fputs(C_BOLD C_RED "Error: " C_RESET, stderr);
//     vfprintf(stderr, fmt, ap);
//     fputc('\n', stderr);
//     va_end(ap);
// }

static void print_info(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fputs(C_BOLD C_GREEN, stdout);
    vfprintf(stdout, fmt, ap);
    fputs(C_RESET "\n", stdout);
    va_end(ap);
}

void enable_ansi_colors(void);

int main(void)
{
    enable_ansi_colors();

    printf(C_BOLD C_CYAN "    C Python Shell 2025\n\n" C_RESET);
    printf(C_GREEN "    Created by Fedya, Danya, Staryi\n" C_RESET);
    printf(C_YELLOW "    For help - help()\n" C_RESET);

    while(1) {
        char* line = readline(PROMPT_STR);

        if (!line) break;

        if (*line) {
            add_history(line);
            print_info("%s", line);
        } else {
            print_info("Empty line — nothing to execute");
        }

        free(line);
    }
    return 0;
}

