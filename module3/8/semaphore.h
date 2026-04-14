#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>

int get_file_sem(const char *filename);

void remove_file_sem(int semid);

void sem_wait_op(int semid);

void sem_post_op(int semid);

#endif

