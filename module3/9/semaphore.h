#ifndef SEM_POSIX_H
#define SEM_POSIX_H

#include <semaphore.h>
#include <fcntl.h>

#define SEM_NAME "/file_access_sem"

int sem_create();
void sem_wait_op(sem_t *sem);
void sem_post_op(sem_t *sem);
void sem_cleanup(sem_t *sem);

#endif 

