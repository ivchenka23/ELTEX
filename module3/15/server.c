#include "tcp_calc.h"
#include <sys/select.h>

typedef enum { ST_CMD, ST_FILE } client_state_t;

typedef struct {
    int fd;
    client_state_t state;
    char buf[BUF_SIZE];
    size_t buf_len;
    char fname[256];
    unsigned long fsize;
    unsigned long frecv;
    FILE* fp;
} client_t;

client_t clients[MAX_CLIENTS];

void init_clients(void) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = -1;
        clients[i].state = ST_CMD;
        clients[i].buf_len = 0;
        clients[i].fp = NULL;
    }
}

void remove_client(int i) {
    if (clients[i].fp) fclose(clients[i].fp);
    if (clients[i].fd != -1) close(clients[i].fd);
    clients[i].fd = -1;
    clients[i].state = ST_CMD;
    clients[i].buf_len = 0;
    printf("Client disconnected\n");
}

void process_line(int i, char* line) {
    if (strlen(line) == 0) return;
    char op[16], fname[256];
    double a, b;
    unsigned long fsize;
    int err;

    if (sscanf(line, "%15s %lf %lf", op, &a, &b) == 3) {
        double res = compute_result(op, a, b, &err);
        char resp[BUF_SIZE];
        if (err == 1) snprintf(resp, sizeof(resp), "ERR division_by_zero");
        else if (err == 2) snprintf(resp, sizeof(resp), "ERR unknown_command");
        else snprintf(resp, sizeof(resp), "OK %.2lf", res);
        send_line(clients[i].fd, resp);
    }
    else if (sscanf(line, "FILE %255s %lu", fname, &fsize) == 2) {
        clients[i].fp = fopen(fname, "wb");
        if (!clients[i].fp) {
            send_line(clients[i].fd, "ERR file_create");
            return;
        }
        strncpy(clients[i].fname, fname, sizeof(clients[i].fname) - 1);
        clients[i].fsize = fsize;
        clients[i].frecv = 0;
        clients[i].state = ST_FILE;
        send_line(clients[i].fd, "READY");
    }
    else {
        send_line(clients[i].fd, "ERR invalid_format");
    }
}

void handle_client_data(int i) {
    char temp[BUF_SIZE];
    ssize_t n = recv(clients[i].fd, temp, sizeof(temp), 0);

    if (n <= 0) {
        remove_client(i);
        return;
    }

    if (clients[i].state == ST_CMD) {
        size_t space = sizeof(clients[i].buf) - clients[i].buf_len - 1;
        size_t to_copy = n < space ? n : space;
        memcpy(clients[i].buf + clients[i].buf_len, temp, to_copy);
        clients[i].buf_len += to_copy;
        clients[i].buf[clients[i].buf_len] = '\0';

        char* p = clients[i].buf;
        while (1) {
            char* newline = strchr(p, '\n');
            if (!newline) break;
            *newline = '\0';
            process_line(i, p);
            p = newline + 1;
        }
        size_t remaining = clients[i].buf_len - (p - clients[i].buf);
        memmove(clients[i].buf, p, remaining + 1);
        clients[i].buf_len = remaining;
    }
    else if (clients[i].state == ST_FILE) {
        size_t to_write = n;
        if (clients[i].frecv + to_write > clients[i].fsize)
            to_write = clients[i].fsize - clients[i].frecv;

        if (to_write > 0 && clients[i].fp) {
            fwrite(temp, 1, to_write, clients[i].fp);
            clients[i].frecv += to_write;
        }

        if (clients[i].frecv >= clients[i].fsize) {
            if (clients[i].fp) fclose(clients[i].fp);
            clients[i].fp = NULL;
            send_line(clients[i].fd, "OK file_received");
            clients[i].state = ST_CMD;
            clients[i].buf_len = 0;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        exit(1);
    }

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); exit(1); }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind"); exit(1);
    }
    if (listen(listen_fd, 5) < 0) {
        perror("listen"); exit(1);
    }

    init_clients();
    printf("Server (multiplexed) listening on port %s\n", argv[1]);

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);
        int maxfd = listen_fd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd != -1) {
                FD_SET(clients[i].fd, &readfds);
                if (clients[i].fd > maxfd) maxfd = clients[i].fd;
            }
        }

        int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            if (errno == EINTR) continue;
            perror("select"); break;
        }

        if (FD_ISSET(listen_fd, &readfds)) {
            struct sockaddr_in cli_addr;
            socklen_t clilen = sizeof(cli_addr);
            int newfd = accept(listen_fd, (struct sockaddr*)&cli_addr, &clilen);
            if (newfd >= 0) {
                int slot = -1;
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i].fd == -1) { slot = i; break; }
                }
                if (slot != -1) {
                    clients[slot].fd = newfd;
                    printf("New client connected (slot %d)\n", slot);
                }
                else {
                    close(newfd);
                    fprintf(stderr, "Max clients reached\n");
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd != -1 && FD_ISSET(clients[i].fd, &readfds)) {
                handle_client_data(i);
            }
        }
    }

    close(listen_fd);
    return 0;
}
