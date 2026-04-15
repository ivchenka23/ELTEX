#include "tcp_calc.h"
#include <sys/wait.h>

double compute(const char *op, double a, double b, int *err) {
    char top[16] = {0};
    for (size_t i = 0; i < strlen(op); i++) top[i] = toupper((unsigned char)op[i]);
    *err = 0;
    if (strcmp(top, "DIFF") == 0) return a - b;
    if (strcmp(top, "PROD") == 0) return a * b;
    if (strcmp(top, "QUOT") == 0) {
        if (b == 0) { *err = 1; return 0; }
        return a / b;
    }
    *err = 2;
    return 0;
}

void dostuff(int sock) {
    char cmd[BUF_SIZE], resp[BUF_SIZE];
    while (1) {
        if (recv_line(sock, cmd, BUF_SIZE) < 0) break;
        if (strlen(cmd) == 0) break;

        char op[16], fname[256];
        double a, b;
        unsigned long fsize;
        int err;

        if (sscanf(cmd, "%15s %lf %lf", op, &a, &b) == 3) {
            double res = compute(op, a, b, &err);
            if (err == 1) { send_line(sock, "ERR division_by_zero"); continue; }
            if (err == 2) { send_line(sock, "ERR unknown_command"); continue; }
            snprintf(resp, BUF_SIZE, "OK %.2lf", res);
            if (send_line(sock, resp) < 0) break;
        }
        else if (sscanf(cmd, "FILE %255s %lu", fname, &fsize) == 2) {
            if (send_line(sock, "READY") < 0) break;
            FILE *f = fopen(fname, "wb");
            if (!f) { send_line(sock, "ERR file_create"); continue; }
            char buf[BUF_SIZE];
            unsigned long to_read = fsize;
            int xfer_err = 0;
            while (to_read > 0) {
                unsigned long chunk = to_read > BUF_SIZE ? BUF_SIZE : to_read;
                if (recv_all(sock, buf, chunk) < 0) { xfer_err = 1; break; }
                fwrite(buf, 1, chunk, f);
                to_read -= chunk;
            }
            fclose(f);
            if (xfer_err) { send_line(sock, "ERR transfer_failed"); continue; }
            if (send_line(sock, "OK file_received") < 0) break;
        }
        else {
            send_line(sock, "ERR invalid_format");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind"); exit(1);
    }
    if (listen(sockfd, 5) < 0) {
        perror("listen"); exit(1);
    }

    printf("Server listening on port %s\n", argv[1]);
    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) { perror("accept"); continue; }

        pid_t pid = fork();
        if (pid < 0) { perror("fork"); close(newsockfd); continue; }
        if (pid == 0) {
            close(sockfd);
            dostuff(newsockfd);
            close(newsockfd);
            exit(0);
        }
        close(newsockfd);
    }
    close(sockfd);
    return 0;
}
