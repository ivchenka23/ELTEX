#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include "shared.h"

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

static volatile sig_atomic_t running = 1;

static void handler(int sig) {
    (void)sig;
    running = 0;
}

static int sem_P(int semid, int semnum) {
    struct sembuf op = { .sem_num = semnum, .sem_op = -1, .sem_flg = 0 };
    return semop(semid, &op, 1);
}

static int sem_V(int semid, int semnum) {
    struct sembuf op = { .sem_num = semnum, .sem_op = 1, .sem_flg = 0 };
    return semop(semid, &op, 1);
}

int main(void) {
    key_t shm_key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    if (shm_key == -1) {
        perror("ftok shm");
        return 1;
    }

    int shmid = shmget(shm_key, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    SharedData *shm = shmat(shmid, NULL, 0);
    if (shm == (void *)-1) {
        perror("shmat");
        return 1;
    }

    key_t sem_key = ftok(SHM_KEY_PATH, 0xBB);
    if (sem_key == -1) {
        perror("ftok sem");
        return 1;
    }

    int semid = semget(sem_key, 2, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        return 1;
    }

    union semun su;
    su.val = 0;
    if (semctl(semid, 0, SETVAL, su) == -1 || semctl(semid, 1, SETVAL, su) == -1) {
        perror("semctl init");
        return 1;
    }

    signal(SIGINT, handler);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        while (running) {
            if (sem_P(semid, 0) == -1) {
                if (errno == EINTR && !running) break;
                perror("sem_P child");
                break;
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
            sem_V(semid, 1);
        }
        shmdt(shm);
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

            sem_V(semid, 0);

            if (sem_P(semid, 1) == -1) {
                if (errno == EINTR && !running) break;
                perror("sem_P parent");
                break;
            }
            if (!running) break;

            printf("  → min=%d, max=%d\n", shm->min_val, shm->max_val);
            processed++;
        }

        printf("\n[Parent] Получен SIGINT.\n");
        printf("Обработано наборов: %d\n", processed);

        wait(NULL);

        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, IPC_RMID, 0);
    }

    return 0;
}
