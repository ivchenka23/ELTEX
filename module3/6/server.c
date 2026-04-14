#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "chat.h"

static int msqid = -1;
static int active_clients[MAX_CLIENTS];  
static int running = 1;

static void cleanup(int sig) {
    (void)sig;
    running = 0;
}

int main(void) {
    key_t key = ftok(IPC_KEY_PATH, IPC_KEY_ID);
    if (key == -1) {
        perror("ftok");
        return 1;
    }

    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("msgget");
        return 1;
    }

    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);

    memset(active_clients, 0, sizeof(active_clients));
    printf("[Server] Очередь создана (msqid=%d). Ожидание сообщений...\n", msqid);
    printf("[Server] Для остановки нажмите Ctrl+C\n\n");

    Message msg;
    while (running) {
        ssize_t len = msgrcv(msqid, &msg, sizeof(msg) - sizeof(long),
                             SERVER_TYPE, 0);
        if (len == -1) {
            if (!running) break;
            perror("msgrcv");
            continue;
        }

        if (strcmp(msg.text, "shutdown") == 0) {
            printf("[Server] Клиент %d отключился (shutdown)\n", msg.sender);
            active_clients[msg.sender] = 0;
            continue;
        }

        if (!active_clients[msg.sender]) {
            active_clients[msg.sender] = 1;
            printf("[Server] Клиент %d подключился\n", msg.sender);
        }

        for (int i = 1; i < MAX_CLIENTS; i++) {
            if (active_clients[i] && i != msg.sender) {
                Message fwd = msg;
                fwd.mtype = CLIENT_TYPE(i);
                if (msgsnd(msqid, &fwd, sizeof(fwd) - sizeof(long), 0) == -1) {
                    perror("msgsnd (forward)");
                }
            }
        }
    }

    msgctl(msqid, IPC_RMID, NULL);
    printf("\n[Server] Очередь удалена. Завершение.\n");
    return 0;
}

