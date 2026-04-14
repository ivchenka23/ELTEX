#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "chat.h"

static int msqid;
static int my_id;
static int running = 1;

static void *reader_thread(void *arg) {
    (void)arg;
    Message msg;

    while (running) {
        ssize_t len = msgrcv(msqid, &msg, sizeof(msg) - sizeof(long),
                             CLIENT_TYPE(my_id), IPC_NOWAIT);
        if (len > 0) {
            printf("\n[Клиент %d] %s\n> ", msg.sender, msg.text);
            fflush(stdout);
        } else {
            usleep(100000);  
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <номер_клиента>\n", argv[0]);
        return 1;
    }

    my_id = atoi(argv[1]);
    if (my_id < 1 || my_id >= MAX_CLIENTS) {
        fprintf(stderr, "Номер клиента должен быть от 1 до %d\n", MAX_CLIENTS - 1);
        return 1;
    }

    key_t key = ftok(IPC_KEY_PATH, IPC_KEY_ID);
    if (key == -1) {
        perror("ftok");
        return 1;
    }

    msqid = msgget(key, 0666);
    if (msqid == -1) {
        perror("msgget (возможно, сервер не запущен?)");
        return 1;
    }

    printf("[Клиент %d] Подключён к очереди (msqid=%d)\n", my_id, msqid);
    printf("[Клиент %d] Введите сообщение или «exit» для выхода\n\n> ", my_id);
    fflush(stdout);

    pthread_t tid;
    pthread_create(&tid, NULL, reader_thread, NULL);

    char line[MAX_TEXT];
    while (running && fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;

        if (line[0] == '\0') {
            printf("> ");
            fflush(stdout);
            continue;
        }

        if (strcmp(line, "exit") == 0) {
            printf("[Клиент %d] Отправка shutdown и выход.\n", my_id);
            Message bye;
            bye.mtype = SERVER_TYPE;
            bye.sender = my_id;
            strncpy(bye.text, "shutdown", sizeof(bye.text));
            msgsnd(msqid, &bye, sizeof(bye) - sizeof(long), 0);
            break;
        }

        Message msg;
        msg.mtype = SERVER_TYPE;
        msg.sender = my_id;
        strncpy(msg.text, line, sizeof(msg.text));
        msg.text[sizeof(msg.text) - 1] = '\0';

        if (msgsnd(msqid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            break;
        }

        printf("> ");
        fflush(stdout);
    }

    running = 0;
    pthread_join(tid, NULL);
    printf("\n[Клиент %d] Завершён.\n", my_id);
    return 0;
}

