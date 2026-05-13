#ifndef UDP_CHAT_H
#define UDP_CHAT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1024

typedef struct {
    int sockfd;
    struct sockaddr_in peer_addr;
    volatile sig_atomic_t *running;
    pthread_mutex_t *lock;
} ChatContext;

void *receiver_thread(void *arg);
void *sender_thread(void *arg);

#endif
