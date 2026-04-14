#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "semaphore.h"

#define MAX_LINE 1024

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <файл>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    int semid = get_file_sem(filename);
    if (semid == -1) return 1;

    FILE *fp = fopen(filename, "r+");
    if (!fp) {
        fp = fopen(filename, "w+");
        if (!fp) {
            perror("fopen");
            return 1;
        }
    }

    printf("[Consumer %d] Ожидание данных в файле %s...\n", getpid(), filename);

    char line[MAX_LINE];

    while (1) {
        sem_wait_op(semid);
        fseek(fp, 0, SEEK_SET);

        long found_pos = -1;
        while (fgets(line, sizeof(line), fp)) {
            if (line[0] != '#') {
                found_pos = ftell(fp) - strlen(line);
                break;
            }
        }

        if (found_pos >= 0) {
            fseek(fp, found_pos, SEEK_SET);
            fgets(line, sizeof(line), fp);
            line[strcspn(line, "\n")] = 0;

            fseek(fp, found_pos, SEEK_SET);
            fprintf(fp, "#%s\n", line);
            fflush(fp);

            int max_val = INT_MIN;
            int min_val = INT_MAX;
            int found = 0;
            char *token = strtok(line, " ");

            while (token) {
                int val = atoi(token);
                if (val > max_val) max_val = val;
                if (val < min_val) min_val = val;
                found = 1;
                token = strtok(NULL, " ");
            }

            if (found) {
                printf("[Consumer %d] min=%d, max=%d (строка: %s)\n",
                       getpid(), min_val, max_val, line);
            }
        } else {
            printf("[Consumer %d] Нет новых строк, ожидание...\n", getpid());
        }

        sem_post_op(semid);
        usleep(500000);
    }

    fclose(fp);
    remove_file_sem(semid);
    return 0;
}

