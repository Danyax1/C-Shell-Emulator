#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "handle_io.h"
#include "colors.h"

#define MAX_BLOCK_SIZE 8192
#define MAX_FILE_SIZE 65536

void enable_ansi_colors(void);

static char* read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return NULL;
    }

    char *content = malloc(MAX_FILE_SIZE);
    if (!content) {
        fclose(file);
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    size_t total = 0;
    size_t bytes_read;
    
    while ((bytes_read = fread(content + total, 1, MAX_FILE_SIZE - total - 1, file)) > 0) {
        total += bytes_read;
        if (total >= MAX_FILE_SIZE - 1) {
            fprintf(stderr, "Error: File too large (max %d bytes)\n", MAX_FILE_SIZE);
            free(content);
            fclose(file);
            return NULL;
        }
    }
    
    content[total] = '\0';
    fclose(file);
    return content;
}

static int run_script(const char *filename) {
    char *content = read_file(filename);
    if (!content) {
        return 1;
    }

    int result = handle_io(content);
    free(content);
    
    return (result == IO_EXIT) ? 1 : 0;
}

static void run_repl(void) {
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
            char line[512] = {0};
            strncat(line, input, 511);
            strcat(line, "\n");
            if (handle_io(line) == IO_EXIT) {
                free(input);
                break;
            }
            free(input);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1) {
        // Script mode: execute file
        return run_script(argv[1]);
    } else {
        // REPL mode: interactive shell
        run_repl();
        return 0;
    }
}