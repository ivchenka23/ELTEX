#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "process.h"

int main(int argc, char *argv[]) {
    int arg_count = argc - 1;

    if (arg_count == 0) {
        printf("Использование: %s <аргумент1> [аргумент2] ...\n", argv[0]);
        return 1;
    }

    printf("Всего аргументов: %d\n", arg_count);
    printf("Аргументы: ");
    for (int i = 1; i < argc; i++) {
        printf("\"%s\" ", argv[i]);
    }
    printf("\n");

    int mid = arg_count / 2;

    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка fork");
        return 1;
    }

    if (pid == 0) {
        handle_args(argc, argv, mid, arg_count, "Дочерний");
        exit(0);
    } else {
        handle_args(argc, argv, 0, mid, "Родительский");

        int status;
        wait(&status);
        printf("\nВсе процессы завершены\n");
    }

    return 0;
}

