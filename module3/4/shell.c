#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "shell.h"

static int split_by_pipe(char *line, char *segments[]) {
    int n = 0;
    segments[n++] = line;

    for (char *p = line; *p; p++) {
        if (*p == '|') {
            *p = '\0';
            if (n < MAX_CMDS)
                segments[n++] = p + 1;
        }
    }
    return n;
}

static void parse_segment(char *seg, Command *cmd) {
    cmd->argc = 0;
    cmd->use_input_file = 0;
    cmd->use_output_file = 0;
    cmd->append = 0;
    cmd->input_file[0] = '\0';
    cmd->output_file[0] = '\0';

    char *token = strtok(seg, " \t\n\r");
    while (token != NULL && cmd->argc < MAX_ARGS - 1) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \t\n\r");
            if (token) {
                strncpy(cmd->input_file, token, MAX_PATH_LEN - 1);
                cmd->input_file[MAX_PATH_LEN - 1] = '\0';
                cmd->use_input_file = 1;
            }
            token = strtok(NULL, " \t\n\r");
            continue;
        }

        if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, " \t\n\r");
            if (token) {
                strncpy(cmd->output_file, token, MAX_PATH_LEN - 1);
                cmd->output_file[MAX_PATH_LEN - 1] = '\0';
                cmd->use_output_file = 1;
                cmd->append = 1;
            }
            token = strtok(NULL, " \t\n\r");
            continue;
        }

        if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t\n\r");
            if (token) {
                strncpy(cmd->output_file, token, MAX_PATH_LEN - 1);
                cmd->output_file[MAX_PATH_LEN - 1] = '\0';
                cmd->use_output_file = 1;
                cmd->append = 0;
            }
            token = strtok(NULL, " \t\n\r");
            continue;
        }

        cmd->argv[cmd->argc++] = token;
        token = strtok(NULL, " \t\n\r");
    }
    cmd->argv[cmd->argc] = NULL;
}

int parse_pipeline(char *line, Command cmds[]) {
    char *segments[MAX_CMDS];
    int n = split_by_pipe(line, segments);

    for (int i = 0; i < n; i++) {
        parse_segment(segments[i], &cmds[i]);
    }
    return n;
}

void execute_pipeline(Command cmds[], int n) {
    if (n == 0 || cmds[0].argv[0] == NULL)
        return;

    int pipefd[MAX_CMDS - 1][2];
    pid_t pids[MAX_CMDS];

    for (int i = 0; i < n; i++) {
        if (i < n - 1) {
            if (pipe(pipefd[i]) == -1) {
                perror("Ошибка pipe");
                return;
            }
        }

        pids[i] = fork();

        if (pids[i] == 0) {
            if (cmds[i].use_input_file) {
                int fd = open(cmds[i].input_file, O_RDONLY);
                if (fd == -1) {
                    perror("Ошибка открытия файла ввода");
                    _exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            } else if (i > 0) {
                dup2(pipefd[i - 1][0], STDIN_FILENO);
                close(pipefd[i - 1][0]);
                close(pipefd[i - 1][1]);
            }

            if (cmds[i].use_output_file) {
                int flags = O_WRONLY | O_CREAT;
                flags |= cmds[i].append ? O_APPEND : O_TRUNC;
                int fd = open(cmds[i].output_file, flags, 0644);
                if (fd == -1) {
                    perror("Ошибка открытия файла вывода");
                    _exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            } else if (i < n - 1) {
                dup2(pipefd[i][1], STDOUT_FILENO);
                close(pipefd[i][0]);
                close(pipefd[i][1]);
            }

            execvp(cmds[i].argv[0], cmds[i].argv);
            execvp(cmds[i].argv[0], cmds[i].argv);

            fprintf(stderr, "%s: команда не найдена\n", cmds[i].argv[0]);
            _exit(127);
        }

        if (i > 0) {
            close(pipefd[i - 1][0]);
            close(pipefd[i - 1][1]);
        }
    }

    if (n > 1) {
        close(pipefd[n - 2][0]);
        close(pipefd[n - 2][1]);
    }

    for (int i = 0; i < n; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }
}

