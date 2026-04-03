#include "calculator.h"
#include <math.h>

static double impl(double base, double exp, int* error) {
    *error = 0;
    return pow(base, exp);
}

int get_command(Command* cmd) {
    cmd->func = impl;
    cmd->args = 2;
    snprintf(cmd->name, sizeof(cmd->name), "Возведение в степень");
    snprintf(cmd->symbol, sizeof(cmd->symbol), "^");
    return 0;
}
