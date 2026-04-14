#include "semaphore.h"
#include <stdio.h>
#include <stdlib.h>

int sem_create() {
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return -1;
    }
    return 0;
}

void sem_wait_op(sem_t *sem) {
    sem_wait(sem);
}

void sem_post_op(sem_t *sem) {
    sem_post(sem);
}

void sem_cleanup(sem_t *sem) {
    sem_close(sem);
    sem_unlink(SEM_NAME);
}

