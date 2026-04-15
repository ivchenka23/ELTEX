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

static inline int send_all(int fd, const void *buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, (char *)buf + sent, len - sent, 0);
        if (n <= 0) return -1;
        sent += n;
    }
    return 0;
}

static inline int recv_all(int fd, void *buf, size_t len) {
    size_t recvd = 0;
    while (recvd < len) {
        ssize_t n = recv(fd, (char *)buf + recvd, len - recvd, 0);
        if (n <= 0) return -1;
        recvd += n;
    }
    return 0;
}

static inline int send_line(int fd, const char *str) {
    size_t len = strlen(str);
    if (send_all(fd, str, len) < 0 || send_all(fd, "\n", 1) < 0) return -1;
    return 0;
}

static inline int recv_line(int fd, char *buf, size_t maxlen) {
    size_t i = 0;
    char c;
    while (i < maxlen - 1) {
        if (recv_all(fd, &c, 1) < 0) return -1;
        if (c == '\n') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return 0;
}

#endif
