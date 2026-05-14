#include "common.h"
#include <time.h>

static volatile sig_atomic_t running = 1;
static int sockfd = -1;
static struct sockaddr_in server_addr;
static uint16_t my_port;

static void sig_handler(int sig) { 
    (void)sig; running = 0; 
    if (sockfd != -1) {
        uint8_t close_buf[PKT_BUF];
        int slen;
        struct sockaddr_in local = {0};
        local.sin_family = AF_INET;
        local.sin_port = my_port;
        local.sin_addr.s_addr = INADDR_ANY;
        if (build_udp_packet(&local, &server_addr, CLOSE_MSG, strlen(CLOSE_MSG), close_buf, &slen) == 0) {
            sendto(sockfd, close_buf, slen, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1) { perror("socket"); return 1; }

    int hdrincl = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &hdrincl, sizeof(hdrincl)) == -1) {
        perror("setsockopt"); close(sockfd); return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("inet_pton"); close(sockfd); return 1;
    }

    srand(time(NULL) ^ getpid());
    my_port = htons(40000 + (rand() % 20000));

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    fd_set readfds;
    uint8_t recv_buf[PKT_BUF];
    char input[PAYLOAD_BUF];
    struct sockaddr_in pkt_src, pkt_dst;
    char payload[PAYLOAD_BUF];
    int plen;

    printf("Client connected to %s:%s (my_port: %u). Ctrl+C to exit.\n> ", 
           argv[1], argv[2], ntohs(my_port));
    fflush(stdout);

    while (running) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sockfd, &readfds);

        int maxfd = STDIN_FILENO > sockfd ? STDIN_FILENO : sockfd;
        int sel = select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if (sel == -1) {
            if (errno == EINTR && !running) break;
            perror("select"); continue;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (fgets(input, sizeof(input), stdin)) {
                input[strcspn(input, "\n")] = '\0';
                if (input[0] != '\0') {
                    struct sockaddr_in local = {0};
                    local.sin_family = AF_INET;
                    local.sin_port = my_port;
                    local.sin_addr.s_addr = INADDR_ANY;
                    uint8_t send_buf[PKT_BUF];
                    int slen;
                    if (build_udp_packet(&local, &server_addr, input, strlen(input), send_buf, &slen) == 0) {
                        sendto(sockfd, send_buf, slen, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                    }
                }
                printf("> "); fflush(stdout);
            }
        }

        if (FD_ISSET(sockfd, &readfds)) {
            int n = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, NULL, NULL);
            if (n > 0 && parse_udp_packet(recv_buf, n, &pkt_src, &pkt_dst, payload, &plen) == 0) {
                if (pkt_src.sin_addr.s_addr == server_addr.sin_addr.s_addr && 
                    pkt_src.sin_port == server_addr.sin_port &&
                    pkt_dst.sin_port == my_port) {
                    payload[plen] = '\0';
                    printf("\r[Server]: %s\n> ", payload);
                    fflush(stdout);
                }
            }
        }
    }

    printf("\nClient terminated.\n");
    close(sockfd);
    return 0;
}
