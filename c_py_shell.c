#include <stdio.h>
#include <string.h>
#include "colors.h"
#include <stdarg.h>

static void print_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fputs(C_BOLD C_RED "Error: " C_RESET, stderr);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
}

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


    char buf[512];
    while(1){
        printf(PROMPT_STR);
        if (!fgets(buf, sizeof(buf), stdin)) break;

        if (buf[0] == '\n') {
            print_info("Empty line — nothing to execute");
            continue;
        }

        if (strcmp(buf, "error\n") == 0) {
            print_error("Unexpected token near '%s'", buf);
            continue;
        }

        print_info("%s", buf);
    }
    return 0;
}
