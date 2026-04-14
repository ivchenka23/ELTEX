#ifndef SHELL_H
#define SHELL_H

#define MAX_LINE 1024
#define MAX_ARGS 64

int parse_line(char *line, char *args[]);

int execute_command(char *args[]);

#endif 

