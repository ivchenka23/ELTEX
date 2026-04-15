#ifndef TCP_CALC_H
#define TCP_CALC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#define BUF_SIZE 4096
#define MAX_CLIENTS 32

static inline int send_all(int fd, const void* buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, (const char*)buf + sent, len - sent, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        sent += n;
    }
    return 0;
}

static inline int send_line(int fd, const char* str) {
    size_t len = strlen(str);
    if (send_all(fd, str, len) < 0 || send_all(fd, "\n", 1) < 0) return -1;
    return 0;
}

static inline double compute_result(const char* op, double a, double b, int* err) {
    char top[16] = { 0 };
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

#endif
