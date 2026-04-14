#include "semaphore.h"
#include <stdio.h>
#include <stdlib.h>

static key_t get_sem_key(const char *filename) {
    char key_path[512];
    snprintf(key_path, sizeof(key_path), "/tmp/sem_%s", filename);
    return ftok(key_path, 'S');
}

int get_file_sem(const char *filename) {
    key_t key = get_sem_key(filename);
    int semid = semget(key, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        return -1;
    }

    unsigned short val = 1;
    if (semctl(semid, 0, SETVAL, val) == -1) {
        perror("semctl SETVAL");
        return -1;
    }

    return semid;
}

void remove_file_sem(int semid) {
    semctl(semid, 0, IPC_RMID);
}

void sem_wait_op(int semid) {
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);
}

void sem_post_op(int semid) {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}

