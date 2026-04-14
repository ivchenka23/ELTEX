#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "shell.h"

int parse_line(char *line, char *args[]) {
    int count = 0;
    char *token = strtok(line, " \t\n\r");

    while (token != NULL && count < MAX_ARGS - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t\n\r");
    }
    args[count] = NULL;
    return count;
}

int execute_command(char *args[]) {
    if (args[0] == NULL)
        return 0;

    if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0)
        return -1;

    pid_t pid = fork();
    if (pid < 0) {
        perror("Ошибка fork");
        return 0;
    }

    if (pid == 0) {

        execvp(args[0], args);

        char *path = getenv("PATH");
        if (path != NULL) {
            execvp(args[0], args);
        }

        fprintf(stderr, "%s: команда не найдена\n", args[0]);
        exit(127);
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        return 0;
    }
}

