#include "calculator.h"

static double impl(double a, double b, int* error) {
    if (b == 0) {
        printf("Ошибка: деление на ноль\n");
        *error = 1;
        return 0;
    }
    *error = 0;
    return a / b;
}

int get_command(Command* cmd) {
    cmd->func = impl;
    cmd->args = 2;
    snprintf(cmd->name, sizeof(cmd->name), "Деление");
    snprintf(cmd->symbol, sizeof(cmd->symbol), "/");
    return 0;
}
