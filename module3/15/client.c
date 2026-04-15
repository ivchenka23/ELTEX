#define _POSIX_C_SOURCE 200809L
#include "tcp_calc.h"
#include <netdb.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(1);
    }

    struct addrinfo hints = { 0 }, * res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(argv[1], argv[2], &hints, &res) != 0) {
        fprintf(stderr, "ERROR, no such host or invalid port\n");
        exit(1);
    }

    int my_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (my_sock < 0) { perror("socket"); freeaddrinfo(res); exit(1); }

    if (connect(my_sock, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect"); freeaddrinfo(res); exit(1);
    }
    freeaddrinfo(res);

    char buff[BUF_SIZE], resp[BUF_SIZE];
    while (1) {
        printf("> ");
        if (fgets(buff, BUF_SIZE, stdin) == NULL) break;
        buff[strcspn(buff, "\n")] = 0;
        if (strlen(buff) == 0) continue;
        if (strcmp(buff, "exit") == 0) break;

        char op[16];
        double a, b;
        if (sscanf(buff, "%15s %lf %lf", op, &a, &b) == 3) {
            char cmd[BUF_SIZE];
            snprintf(cmd, BUF_SIZE, "%s %g %g", op, a, b);
            if (send_line(my_sock, cmd) < 0) break;
            if (recv(my_sock, resp, BUF_SIZE, 0) <= 0) break;
            resp[strcspn(resp, "\n")] = 0;
            printf("%s\n", resp);
        }
        else if (strncmp(buff, "file ", 5) == 0 || strncmp(buff, "FILE ", 5) == 0) {
            char fname[256];
            sscanf(buff, "%*s %255s", fname);
            FILE* f = fopen(fname, "rb");
            if (!f) { printf("ERR file_not_found\n"); continue; }
            fseek(f, 0, SEEK_END);
            long fsize = ftell(f);
            rewind(f);

            char cmd[BUF_SIZE];
            snprintf(cmd, BUF_SIZE, "FILE %s %ld", fname, fsize);
            if (send_line(my_sock, cmd) < 0) break;
            if (recv(my_sock, resp, BUF_SIZE, 0) <= 0) break;
            resp[strcspn(resp, "\n")] = 0;
            if (strcmp(resp, "READY") != 0) { printf("Server not ready\n"); fclose(f); continue; }

            char buf[BUF_SIZE];
            size_t n;
            while ((n = fread(buf, 1, BUF_SIZE, f)) > 0) {
                if (send_all(my_sock, buf, n) < 0) break;
            }
            fclose(f);
            if (recv(my_sock, resp, BUF_SIZE, 0) <= 0) break;
            resp[strcspn(resp, "\n")] = 0;
            printf("%s\n", resp);
        }
        else {
            printf("ERR unknown_command\n");
        }
    }
    close(my_sock);
    return 0;
}
