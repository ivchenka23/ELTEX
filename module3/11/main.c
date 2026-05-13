#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>
#include "shared.h"

static volatile sig_atomic_t running = 1;
static SharedData *shm_global = NULL;

static void handler(int sig) {
    (void)sig;
    running = 0;
    if (shm_global) {
        sem_post(&shm_global->data_ready);
        sem_post(&shm_global->result_ready);
    }
}

int main(void) {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) { perror("shm_open"); return 1; }

    if (ftruncate(fd, sizeof(SharedData)) == -1) {
        perror("ftruncate"); close(fd); shm_unlink(SHM_NAME); return 1;
    }

    SharedData *shm = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap"); close(fd); shm_unlink(SHM_NAME); return 1;
    }
    shm_global = shm;

    if (sem_init(&shm->data_ready, 1, 0) == -1 || sem_init(&shm->result_ready, 1, 0) == -1) {
        perror("sem_init"); munmap(shm, sizeof(SharedData)); close(fd); shm_unlink(SHM_NAME); return 1;
    }

    signal(SIGINT, handler);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        while (running) {
            if (sem_wait(&shm->data_ready) == -1) {
                if (errno == EINTR) break;
                perror("sem_wait child"); break;
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
            sem_post(&shm->result_ready);
        }
        munmap(shm, sizeof(SharedData));
        close(fd);
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
            for (int i = 0; i < shm->count; i++) printf("%d ", shm->data[i]);
            printf("\n");

            sem_post(&shm->data_ready);

            if (sem_wait(&shm->result_ready) == -1) {
                if (errno == EINTR) break;
                perror("sem_wait parent"); break;
            }
            if (!running) break;

            printf("  -> min=%d, max=%d\n", shm->min_val, shm->max_val);
            processed++;
        }

        printf("\n[Parent] Получен SIGINT.\n");
        printf("Обработано наборов: %d\n", processed);

        wait(NULL);

        sem_destroy(&shm->data_ready);
        sem_destroy(&shm->result_ready);
        munmap(shm, sizeof(SharedData));
        close(fd);
        shm_unlink(SHM_NAME);
    }
    return 0;
}
