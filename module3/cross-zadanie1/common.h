#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/select.h>
#include <errno.h>
#include <stdint.h>

#define MAX_CLIENTS 64
#define PAYLOAD_BUF 1024
#define PKT_BUF (sizeof(struct iphdr) + sizeof(struct udphdr) + PAYLOAD_BUF)
#define CLOSE_MSG "CLOSE"

typedef struct {
    uint32_t ip;
    uint16_t port;
    int count;
} ClientEntry;

uint16_t calc_checksum(uint16_t *addr, size_t len);
int build_udp_packet(struct sockaddr_in *src, struct sockaddr_in *dst, 
                     const char *payload, int plen, uint8_t *buf, int *buflen);
int parse_udp_packet(uint8_t *buf, int len, struct sockaddr_in *src, 
                     struct sockaddr_in *dst, char *payload, int *plen);

#endif
