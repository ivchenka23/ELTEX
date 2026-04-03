#include "calculator.h"
#include <math.h>

static double impl(double x, double unused, int* error) {
    if (x < 0) {
        printf("Ошибка: квадратный корень из отрицательного числа\n");
        *error = 1;
        return 0;
    }
    *error = 0;
    return sqrt(x);
}

int get_command(Command* cmd) {
    cmd->func = impl;
    cmd->args = 1;
    snprintf(cmd->name, sizeof(cmd->name), "Квадратный корень");
    snprintf(cmd->symbol, sizeof(cmd->symbol), "√");
    return 0;
}
