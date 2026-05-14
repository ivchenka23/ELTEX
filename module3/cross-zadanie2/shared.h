#ifndef SHARED_H
#define SHARED_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/epoll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/wait.h>
#include <errno.h>

#define CLI_QUEUE_KEY 0x54415849
#define MAX_MSG_TEXT 256
#define MAX_DRIVERS 64

typedef struct {
    long mtype;
    char mtext[MAX_MSG_TEXT];
} Msg;

typedef struct {
    pid_t pid;
    int msgid;
} Driver;

extern int cli_msgid;
extern int pipe_fds[2];
extern volatile sig_atomic_t running;

void driver_proc(int msgid);
void *receiver_thread(void *arg);
void sig_handler(int sig);

#endif
