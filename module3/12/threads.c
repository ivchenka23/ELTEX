#include "udp_chat.h"

void *sender_thread(void *arg) {
    ChatContext *ctx = (ChatContext *)arg;
    char buf[BUF_SIZE];

    while (*ctx->running) {
        if (fgets(buf, sizeof(buf), stdin)) {
            buf[strcspn(buf, "\n")] = '\0';
            if (buf[0] != '\0') {
                sendto(ctx->sockfd, buf, strlen(buf), 0,
                       (struct sockaddr *)&ctx->peer_addr, sizeof(ctx->peer_addr));
            }
        }
    }
    return NULL;
}

void *receiver_thread(void *arg) {
    ChatContext *ctx = (ChatContext *)arg;
    char buf[BUF_SIZE];

    while (*ctx->running) {
        ssize_t n = recvfrom(ctx->sockfd, buf, sizeof(buf) - 1, 0, NULL, NULL);
        if (n > 0) {
            buf[n] = '\0';
            pthread_mutex_lock(ctx->lock);
            printf("\r[Peer]: %s\n> ", buf);
            fflush(stdout);
            pthread_mutex_unlock(ctx->lock);
        }
    }
    return NULL;
}
