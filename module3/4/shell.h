#ifndef SHELL_H
#define SHELL_H

#include <sys/types.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define MAX_CMDS 32
#define MAX_PATH_LEN 256

typedef struct {
    char *argv[MAX_ARGS];
    int    argc;
    char   input_file[MAX_PATH_LEN];
    char   output_file[MAX_PATH_LEN];
    int    append;
    int    use_input_file;
    int    use_output_file;
} Command;

int parse_pipeline(char *line, Command cmds[]);
void execute_pipeline(Command cmds[], int n);

#endif 

