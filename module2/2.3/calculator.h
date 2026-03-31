#ifndef CALCULATOR_H
#define CALCULATOR_H

#define MAX_COMMANDS 10

typedef double (*MathFunc)(double, double, int*);

typedef struct {
    char name[60];
    char symbol[5];
    MathFunc func;
    int args;
} Command;

double add(double a, double b, int* error);
double subtract(double a, double b, int* error);
double multiply(double a, double b, int* error);
double divide(double a, double b, int* error);
double power(double base, double exp, int* error);
double squareRoot(double x, double unused, int* error);

void initCommands(Command commands[], int* count);
void showMenu(Command commands[], int count);
double executeCommand(Command commands[], int index, double a, double b, int* error);

#endif
