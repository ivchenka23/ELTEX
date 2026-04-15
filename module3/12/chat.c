#include "udp_chat.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <local_port> <remote_ip> <remote_port>\n", argv[0]);
        return 1;
    }

    int local_port = atoi(argv[1]);
    const char *remote_ip = argv[2];
    int remote_port = atoi(argv[3]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); return 1; }

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(local_port);

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind"); return 1;
    }

    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(remote_port);
    inet_pton(AF_INET, remote_ip, &remote_addr.sin_addr);

    char buf[BUF_SIZE];
    fd_set read_fds;
    struct sockaddr_in src_addr;
    socklen_t addrlen = sizeof(src_addr);

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(sockfd, &read_fds);

        int max_fd = STDIN_FILENO > sockfd ? STDIN_FILENO : sockfd;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select"); break;
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(buf, BUF_SIZE, stdin) == NULL) break;
            buf[strcspn(buf, "\n")] = 0;
            if (strcmp(buf, "exit") == 0) break;
            sendto(sockfd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
        }

        if (FD_ISSET(sockfd, &read_fds)) {
            int n = recvfrom(sockfd, buf, BUF_SIZE - 1, 0, (struct sockaddr *)&src_addr, &addrlen);
            if (n > 0) {
                buf[n] = 0;
                printf("[Peer] %s\n", buf);
                printf("> ");
                fflush(stdout);
            }
        }
    }

    close(sockfd);
    return 0;
}
