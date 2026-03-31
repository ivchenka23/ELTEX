#include <stdio.h>
#include <math.h>
#include "calculator.h"

double add(double a, double b, int* error) {
    *error = 0;
    return a + b;
}

double subtract(double a, double b, int* error) {
    *error = 0;
    return a - b;
}

double multiply(double a, double b, int* error) {
    *error = 0;
    return a * b;
}

double divide(double a, double b, int* error) {
    if (b == 0) {
        printf("Ошибка: деление на ноль\n");
        *error = 1;
        return 0;
    }
    *error = 0;
    return a / b;
}

double power(double base, double exp, int* error) {
    *error = 0;
    return pow(base, exp);
}

double squareRoot(double x, double unused, int* error) {
    if (x < 0) {
        printf("Ошибка: квадратный корень из отрицательного числа\n");
        *error = 1;
        return 0;
    }
    *error = 0;
    return sqrt(x);
}

void initCommands(Command commands[], int* count) {
    int i = 0;

    commands[i].args = 2;
    snprintf(commands[i].name, sizeof(commands[i].name), "Сложение");
    snprintf(commands[i].symbol, sizeof(commands[i].symbol), "+");
    commands[i].func = add;
    i++;

    commands[i].args = 2;
    snprintf(commands[i].name, sizeof(commands[i].name), "Вычитание");
    snprintf(commands[i].symbol, sizeof(commands[i].symbol), "-");
    commands[i].func = subtract;
    i++;

    commands[i].args = 2;
    snprintf(commands[i].name, sizeof(commands[i].name), "Умножение");
    snprintf(commands[i].symbol, sizeof(commands[i].symbol), "*");
    commands[i].func = multiply;
    i++;

    commands[i].args = 2;
    snprintf(commands[i].name, sizeof(commands[i].name), "Деление");
    snprintf(commands[i].symbol, sizeof(commands[i].symbol), "/");
    commands[i].func = divide;
    i++;

    commands[i].args = 2;
    snprintf(commands[i].name, sizeof(commands[i].name), "Возведение в степень");
    snprintf(commands[i].symbol, sizeof(commands[i].symbol), "^");
    commands[i].func = power;
    i++;

    commands[i].args = 1;
    snprintf(commands[i].name, sizeof(commands[i].name), "Квадратный корень");
    snprintf(commands[i].symbol, sizeof(commands[i].symbol), "√");
    commands[i].func = squareRoot;
    i++;

    *count = i;
}

void showMenu(Command commands[], int count) {
    printf("\nКалькулятор\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s (%s)\n", i + 1, commands[i].name, commands[i].symbol);
    }
    printf("%d. Выход\n", count + 1);
    printf("Выбор: ");
}

double executeCommand(Command commands[], int index, double a, double b, int* error) {
    if (commands[index].args == 1) {
        return commands[index].func(a, 0, error);
    }
    else {
        return commands[index].func(a, b, error);
    }
}
