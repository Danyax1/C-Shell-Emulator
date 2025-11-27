#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "handle_io.h"
#include "colors.h"

#define MAX_BLOCK_SIZE 8192
#define MAX_FILE_SIZE 65536
#define MAX_LINE_SIZE 512

void enable_ansi_colors(void);

static int is_whitespace_only(const char *line) {
    while (*line) {
        if (!isspace((unsigned char)*line)) {
            return 0; // Not whitespace-only
        }
        line++;
    }
    return 1;
}

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
    char line[MAX_LINE_SIZE];

    while (fgets(line, sizeof(line), file)) {

        // skip empty lines
        if (is_whitespace_only(line))
            continue;

        size_t len = strlen(line);

        if (total + len + 2 >= MAX_FILE_SIZE) {
            fprintf(stderr, "Error: File too large\n");
            free(content);
            fclose(file);
            return NULL;
        }

        memcpy(content + total, line, len);
        total += len;
    }

    fclose(file);

    if (total == 0 || content[total - 1] != '\n') {
        content[total++] = '\n';
    }

    content[total] = '\0';
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