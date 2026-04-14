#include <stdio.h>
#include <string.h>
#include "shell.h"

int main(void) {
    char line[MAX_LINE];
    Command cmds[MAX_CMDS];

    printf("Командный интерпретатор с конвейерами\n");
    printf("Поддерживаются: |  <  >  >>\n");
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

        int n = parse_pipeline(line, cmds);
        if (n == 0)
            continue;

        if (n == 1 && cmds[0].argv[0] != NULL &&
            (strcmp(cmds[0].argv[0], "exit") == 0 ||
             strcmp(cmds[0].argv[0], "quit") == 0))
            break;

        execute_pipeline(cmds, n);
    }

    return 0;
}

