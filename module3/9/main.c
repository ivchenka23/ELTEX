#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <limits.h>
#include "semaphore.h"

#define DATA_FILE "data.txt"
#define MAX_LINE 1024
#define NUM_LINES 10

static void child_process(sem_t *sem) {
    printf("[Child %d] Старт.\n", getpid());

    int processed = 0;
    while (1) {
        sem_wait(sem);

        FILE *fp = fopen(DATA_FILE, "r");
        if (!fp) {
            sem_post(sem);
            usleep(100000);
            continue;
        }

        char line[MAX_LINE];
        int found = 0;

        while (fgets(line, sizeof(line), fp)) {
            if (line[0] != '#') {
                found = 1;
                break;
            }
        }

        if (found) {
            long pos = ftell(fp) - strlen(line);
            fclose(fp);

            fp = fopen(DATA_FILE, "r+");
            fseek(fp, pos, SEEK_SET);
            fprintf(fp, "#%s", line);
            fflush(fp);
            fclose(fp);

            int max_val = INT_MIN;
            int min_val = INT_MAX;
            char *token = strtok(line, " \n");

            while (token) {
                int val = atoi(token);
                if (val > max_val) max_val = val;
                if (val < min_val) min_val = val;
                token = strtok(NULL, " \n");
            }

            processed++;
            printf("[Child] Строка #%d: min=%d, max=%d\n", processed, min_val, max_val);
        }

        sem_post(sem);

        if (found == 0) {
            usleep(100000);
        }
    }
}

int main(void) {
    sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED) {
        sem_unlink(SEM_NAME);
        sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
        if (sem == SEM_FAILED) {
            perror("sem_open");
            return 1;
        }
    }

    FILE *fp = fopen(DATA_FILE, "w");
    if (fp) fclose(fp);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        child_process(sem);
    } else {
        printf("[Parent %d] Генерация %d строк...\n", getpid(), NUM_LINES);
        srand(time(NULL));

        for (int i = 0; i < NUM_LINES; i++) {
            sem_wait(sem);

            FILE *out = fopen(DATA_FILE, "a");
            if (out) {
                char line[MAX_LINE] = "";
                int len = 0;
                int nums = rand() % 5 + 3; 

                for (int j = 0; j < nums; j++) {
                    int n = snprintf(line + len, sizeof(line) - len, "%d ", rand() % 1000);
                    len += n;
                }
                line[len - 1] = '\n';

                fprintf(out, "%s", line);
                fflush(out);
                fclose(out);

                printf("[Parent] Записана строка: %s", line);
            }

            sem_post(sem);
            usleep(300000);
        }

        printf("[Parent] Завершение. Жду дочерний процесс...\n");
        wait(NULL);
        sem_cleanup(sem);
        printf("[Parent] Готово.\n");
    }

    return 0;
}

