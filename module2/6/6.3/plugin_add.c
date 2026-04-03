#include "calculator.h"

static double impl(double a, double b, int* error) {
    *error = 0;
    return a + b;
}

int get_command(Command* cmd) {
    cmd->func = impl;
    cmd->args = 2;
    snprintf(cmd->name, sizeof(cmd->name), "Сложение");
    snprintf(cmd->symbol, sizeof(cmd->symbol), "+");
    return 0;
}
