#include <stdio.h>
#include "calculator.h"

int main() {
    Command commands[MAX_COMMANDS];
    int commandCount;
    int choice;
    double a, b, result;
    int error;

    initCommands(commands, &commandCount);

    while (1) {
        showMenu(commands, commandCount);
        scanf("%d", &choice);

        if (choice == commandCount + 1) {
            printf("Выход из программы\n");
            break;
        }

        if (choice < 1 || choice > commandCount) {
            printf("Неверный выбор\n");
            continue;
        }

        int idx = choice - 1;
        error = 0;

        printf("Введите первый аргумент: ");
        scanf("%lf", &a);

        if (commands[idx].args == 2) {
            printf("Введите второй аргумент: ");
            scanf("%lf", &b);
            result = executeCommand(commands, idx, a, b, &error);
            if (!error) {
                printf("Результат: %.2lf %s %.2lf = %.2lf\n", a, commands[idx].symbol, b, result);
            }
        }
        else {
            result = executeCommand(commands, idx, a, 0, &error);
            if (!error) {
                printf("Результат: %s%.2lf = %.2lf\n", commands[idx].symbol, a, result);
            }
        }
    }

    return 0;
}
