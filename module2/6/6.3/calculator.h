#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <stdio.h>
#include <string.h>

#define MAX_COMMANDS 32
#define PLUGIN_DIR "./plugins"

typedef double (*MathFunc)(double, double, int*);

typedef struct {
    char name[60];
    char symbol[5];
    MathFunc func;
    int args;
} Command;

typedef int (*GetCommandFunc)(Command*);

int loadPlugins(const char* dir, Command commands[], int* count, void* handles[]);
void unloadPlugins(void* handles[], int count);

void showMenu(Command commands[], int count);
double executeCommand(Command commands[], int index, double a, double b, int* error);

#endif
