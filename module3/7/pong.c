#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include "pingpong.h"

int main(void) {
    struct mq_attr attr;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = MAX_MSG_LEN;
    attr.mq_curmsgs = 0;

    mqd_t mq_pong = mq_open(PONG_Q_NAME, OFLAGS, Q_PERMS, &attr);
    mqd_t mq_ping = mq_open(PING_Q_NAME, OFLAGS, Q_PERMS, &attr);

    if (mq_pong == (mqd_t)-1 || mq_ping == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    struct mq_attr attr_nb;
    mq_getattr(mq_pong, &attr_nb);
    attr_nb.mq_flags |= O_NONBLOCK;
    mq_setattr(mq_pong, &attr_nb, NULL);

    printf("[PONG] Подключён. Ожидание сообщений от PING...\n");
    printf("[PONG] Введите сообщение (или «exit» для завершения):\n> ");
    fflush(stdout);

    char buf[MAX_MSG_LEN];
    unsigned int prio;

    while (1) {
        ssize_t bytes = mq_receive(mq_pong, buf, MAX_MSG_LEN, &prio);
        if (bytes > 0) {
            buf[bytes] = '\0';
            if (prio == EXIT_PRIO) {
                printf("\n[PONG] PING завершил обмен. Выход.\n");
                break;
            }
            printf("\n  ← [PING] (prio=%u): %s\n> ", prio, buf);
            fflush(stdout);
        }

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000;

        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
            if (fgets(buf, sizeof(buf), stdin) == NULL)
                break;
            buf[strcspn(buf, "\n")] = 0;

            if (buf[0] == '\0') {
                printf("> ");
                fflush(stdout);
                continue;
            }

            if (strcmp(buf, "exit") == 0) {
                mq_send(mq_ping, "exit", 5, EXIT_PRIO);
                printf("[PONG] Отправлен сигнал завершения. Выход.\n");
                break;
            }

            mq_send(mq_ping, buf, strlen(buf), MSG_PRIO);
            printf("> ");
            fflush(stdout);
        }
    }

    mq_close(mq_pong);
    mq_close(mq_ping);
    mq_unlink(PONG_Q_NAME);
    mq_unlink(PING_Q_NAME);
    return 0;
}
