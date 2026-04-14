#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "shared.h"

static volatile sig_atomic_t running = 1;

static void handler(int sig) {
    (void)sig;
    running = 0;
}

int main(void) {
    key_t key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    if (key == -1) {
        perror("ftok");
        return 1;
    }

    int shmid = shmget(key, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    SharedData *shm = (SharedData *)shmat(shmid, NULL, 0);
    if (shm == (void *)-1) {
        perror("shmat");
        return 1;
    }

    memset(shm, 0, sizeof(SharedData));

    signal(SIGINT, handler);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        while (running) {
            while (!shm->producer_done) {
                if (!running) break;
                usleep(10000);
            }
            if (!running) break;

            int min_v = shm->data[0];
            int max_v = shm->data[0];

            for (int i = 1; i < shm->count; i++) {
                if (shm->data[i] < min_v) min_v = shm->data[i];
                if (shm->data[i] > max_v) max_v = shm->data[i];
            }

            shm->min_val = min_v;
            shm->max_val = max_v;
            shm->consumer_done = 1;
        }
        return 0;
    } else {
        int processed = 0;
        srand(time(NULL) ^ getpid());

        while (running) {
            shm->count = rand() % 10 + 3;  
            for (int i = 0; i < shm->count; i++) {
                shm->data[i] = rand() % 1000;
            }

            printf("Набор: ");
            for (int i = 0; i < shm->count; i++) {
                printf("%d ", shm->data[i]);
            }
            printf("\n");

            shm->producer_done = 1;
            shm->consumer_done = 0;

            while (!shm->consumer_done) {
                if (!running) break;
                usleep(10000);
            }
            if (!running) break;

            printf("  → min=%d, max=%d\n", shm->min_val, shm->max_val);
            processed++;

            shm->producer_done = 0;
            usleep(200000);
        }

        printf("\n[Parent] Получен SIGINT.\n");
        printf("Обработано наборов: %d\n", processed);

        wait(NULL);
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}

