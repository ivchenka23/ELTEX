#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "semaphore.h"

#define MAX_LINE 1024
#define MAX_NUMBERS 20

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <файл> [кол-во_строк] [чисел_в_строке]\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int num_lines = (argc > 2) ? atoi(argv[2]) : 5;
    int nums_per_line = (argc > 3) ? atoi(argv[3]) : 5;

    if (num_lines < 1) num_lines = 5;
    if (nums_per_line < 1 || nums_per_line > MAX_NUMBERS) nums_per_line = 5;

    int semid = get_file_sem(filename);
    if (semid == -1) return 1;

    FILE *fp = fopen(filename, "a");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    srand(time(NULL) ^ getpid());

    for (int i = 0; i < num_lines; i++) {
        sem_wait_op(semid);

        char line[MAX_LINE] = "";
        int len = 0;

        for (int j = 0; j < nums_per_line; j++) {
            int val = rand() % 1000;
            int n = snprintf(line + len, sizeof(line) - len, "%d ", val);
            len += n;
        }
        line[len - 1] = '\n';

        fprintf(fp, "%s", line);
        fflush(fp);

        printf("[Producer] Записана строка: %s", line);

        sem_post_op(semid);
        usleep(200000); 
    }

    fclose(fp);
    remove_file_sem(semid);
    printf("[Producer] Завершён.\n");
    return 0;
}

