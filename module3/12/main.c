#include "udp_chat.h"

static volatile sig_atomic_t running = 1;
static pthread_mutex_t console_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_t recv_tid, send_tid;

static void sig_handler(int sig) {
    (void)sig;
    running = 0;
    pthread_cancel(recv_tid);
    pthread_cancel(send_tid);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <local_port> <peer_ip> <peer_port>\n", argv[0]);
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) { perror("socket"); return 1; }

    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(atoi(argv[1]));
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) == -1) {
        perror("bind"); close(sockfd); return 1;
    }

    ChatContext ctx;
    ctx.sockfd = sockfd;
    ctx.peer_addr.sin_family = AF_INET;
    ctx.peer_addr.sin_port = htons(atoi(argv[3]));
    if (inet_pton(AF_INET, argv[2], &ctx.peer_addr.sin_addr) <= 0) {
        perror("inet_pton"); close(sockfd); return 1;
    }
    ctx.running = &running;
    ctx.lock = &console_lock;

    struct sigaction sa;
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    printf("Chat started. Press Ctrl+C to exit.\n> ");
    fflush(stdout);

    pthread_create(&recv_tid, NULL, receiver_thread, &ctx);
    pthread_create(&send_tid, NULL, sender_thread, &ctx);

    pthread_join(recv_tid, NULL);
    pthread_join(send_tid, NULL);

    pthread_mutex_destroy(&console_lock);
    printf("\nChat terminated.\n");
    close(sockfd);
    return 0;
}
