#ifndef SHM_POSIX_H
#define SHM_POSIX_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SHM_NAME "/shm_posix_demo"
#define MAX_NUMBERS 50

typedef struct {
    int data[MAX_NUMBERS];
    int count;
    int min_val;
    int max_val;
    int producer_done;
    int consumer_done;
} SharedData;

#endif 

