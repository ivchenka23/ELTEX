#include <stdio.h>
#include "calculator.h"

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
    } else {
        return commands[index].func(a, b, error);
    }
}

int main() {
    Command commands[MAX_COMMANDS];
    void* pluginHandles[MAX_COMMANDS] = {0};
    int commandCount;
    int choice;
    double a, b, result;
    int error;

    if (loadPlugins(PLUGIN_DIR, commands, &commandCount, pluginHandles) != 0) {
        printf("Не удалось загрузить плагины.\n");
        return 1;
    }

    if (commandCount == 0) {
        printf("В каталоге '%s' не найдено плагинов.\n", PLUGIN_DIR);
        return 1;
    }

    while (1) {
        showMenu(commands, commandCount);
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            printf("Неверный ввод\n");
            continue;
        }

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
        if (scanf("%lf", &a) != 1) {
            while(getchar() != '\n');
            printf("Некорректное число\n");
            continue;
        }

        if (commands[idx].args == 2) {
            printf("Введите второй аргумент: ");
            if (scanf("%lf", &b) != 1) {
                while(getchar() != '\n');
                printf("Некорректное число\n");
                continue;
            }
            result = executeCommand(commands, idx, a, b, &error);
            if (!error) {
                printf("Результат: %.2lf %s %.2lf = %.2lf\n", a, commands[idx].symbol, b, result);
            }
        } else {
            result = executeCommand(commands, idx, a, 0, &error);
            if (!error) {
                printf("Результат: %s%.2lf = %.2lf\n", commands[idx].symbol, a, result);
            }
        }
    }

    unloadPlugins(pluginHandles, commandCount);
    return 0;
}
