#include <stdio.h>
#include <math.h>
#include "calculator.h"

double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    if (b == 0) {
        printf("Ошибка: деление на ноль!\n");
        return 0;
    }
    return a / b;
}

double power(double base, double exp) {
    return pow(base, exp);
}

double squareRoot(double x) {
    if (x < 0) {
        printf("Ошибка: квадратный корень из отрицательного числа!\n");
        return 0;
    }
    return sqrt(x);
}
