#include "common.h"

static volatile sig_atomic_t running = 1;
static ClientEntry clients[MAX_CLIENTS];
static int client_count = 0;

static void sig_handler(int sig) { (void)sig; running = 0; }

static int find_client_index(uint32_t ip, uint16_t port) {
    for (int i = 0; i < client_count; i++)
        if (clients[i].ip == ip && clients[i].port == port) return i;
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_port>\n", argv[0]);
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1) { perror("socket"); return 1; }

    int hdrincl = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &hdrincl, sizeof(hdrincl)) == -1) {
        perror("setsockopt IP_HDRINCL"); close(sockfd); return 1;
    }

    uint16_t server_port = htons(atoi(argv[1]));

    struct sigaction sa;
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    uint8_t recv_buf[PKT_BUF], send_buf[PKT_BUF];
    struct sockaddr_in src_addr, dst_addr;
    char payload[PAYLOAD_BUF];
    int plen;

    printf("Server listening on port %s. Ctrl+C to exit.\n", argv[1]);

    while (running) {
        int n = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, NULL, NULL);
        if (n == -1) {
            if (errno == EINTR) {
                if (!running) break;
                continue;
            }
            perror("recvfrom");
            continue;
        }

        if (parse_udp_packet(recv_buf, n, &src_addr, &dst_addr, payload, &plen) != 0) continue;
        if (dst_addr.sin_port != server_port) continue;

        payload[plen] = '\0';

        if (strcmp(payload, CLOSE_MSG) == 0) {
            int idx = find_client_index(src_addr.sin_addr.s_addr, src_addr.sin_port);
            if (idx != -1) clients[idx] = clients[--client_count];
            continue;
        }

        int idx = find_client_index(src_addr.sin_addr.s_addr, src_addr.sin_port);
        if (idx == -1) {
            if (client_count >= MAX_CLIENTS) continue;
            clients[client_count].ip = src_addr.sin_addr.s_addr;
            clients[client_count].port = src_addr.sin_port;
            clients[client_count].count = 1;
            idx = client_count++;
        } else {
            clients[idx].count++;
        }

        char resp[PAYLOAD_BUF + 32];
        snprintf(resp, sizeof(resp), "%s %d", payload, clients[idx].count);
        
        struct sockaddr_in reply_src = {0};
        reply_src.sin_family = AF_INET;
        reply_src.sin_port = server_port;
        reply_src.sin_addr.s_addr = dst_addr.sin_addr.s_addr;

        int slen;
        if (build_udp_packet(&reply_src, &src_addr, resp, strlen(resp), send_buf, &slen) == 0) {
            sendto(sockfd, send_buf, slen, 0, (struct sockaddr *)&src_addr, sizeof(src_addr));
        }
    }

    printf("\nServer stopped.\n");
    close(sockfd);
    return 0;
}
