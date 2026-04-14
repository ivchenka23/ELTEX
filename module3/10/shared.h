#ifndef SHARED_H
#define SHARED_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define MAX_NUMBERS 50
#define SHM_KEY_PATH "/tmp"
#define SHM_KEY_ID   0xAA

typedef struct {
    int data[MAX_NUMBERS];
    int count;
    int min_val;
    int max_val;
    int producer_done;  
    int consumer_done;   
} SharedData;

#endif 

