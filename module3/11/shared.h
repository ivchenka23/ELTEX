#ifndef SHARED_H
#define SHARED_H

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SHM_NAME "/shm_posix_demo"
#define MAX_NUMBERS 50

typedef struct {
    sem_t data_ready;
    sem_t result_ready;
    int data[MAX_NUMBERS];
    int count;
    int min_val;
    int max_val;
} SharedData;

#endif
