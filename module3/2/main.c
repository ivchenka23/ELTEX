#include <stdio.h>
#include <string.h>
#include "shell.h"

int main(void) {
    char line[MAX_LINE];
    char *args[MAX_ARGS];

    printf("=== Командный интерпретатор ===\n");
    printf("Введите 'exit' или 'quit' для выхода.\n\n");

    while (1) {
        printf("mysh> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }

        if (line[0] == '\n' || line[0] == '\0')
            continue;

        int argc = parse_line(line, args);
        if (argc == 0)
            continue;

        int result = execute_command(args);

        if (result == -1)
            break;
    }

    return 0;
}

