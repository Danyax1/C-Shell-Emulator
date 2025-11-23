#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "handle_io.h"
#include "colors.h"

#define MAX_BLOCK_SIZE 8192

void enable_ansi_colors(void);

int main(void)
{
    enable_ansi_colors();
    rl_bind_key('\t', rl_insert);
    printf(C_BOLD C_CYAN "    C Python Shell 2025\n\n" C_RESET);
    printf(C_GREEN "    Created by Fedya, Danya, Staryi\n" C_RESET);
    printf(C_YELLOW "    For help - help()\n" C_RESET);

    while (1) {
        char *input = readline(PROMPT_STR);

        if (!input) break;

        if (*input) add_history(input);

        // Check for multiline block
        if (ends_with_colon(input)) {
            char block[MAX_BLOCK_SIZE] = {0};

            strncat(block, input, MAX_BLOCK_SIZE - strlen(block) - 1);
            strncat(block, "\n", MAX_BLOCK_SIZE - strlen(block) - 1);

            free(input);

            while (1) {
                char *line = readline("... ");

                if (!line) {
                    printf("\n");
                    break;
                }

                if (is_empty_line(line)) {
                    free(line);
                    break;
                }

                if (*line) add_history(line);

                strncat(block, line, MAX_BLOCK_SIZE - strlen(block) - 1);
                strncat(block, "\n", MAX_BLOCK_SIZE - strlen(block) - 1);

                free(line);
            }

            if (handle_io(block) == IO_EXIT)
                break;
        }
        else {
            if (handle_io(input) == IO_EXIT) {
                free(input);
                break;
            }
            free(input);
        }
    }

    return 0;
}
