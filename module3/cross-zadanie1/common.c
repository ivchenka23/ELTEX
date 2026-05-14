#include "common.h"

uint16_t calc_checksum(uint16_t *addr, size_t len) {
    unsigned long sum = 0;
    while (len > 1) { sum += *addr++; len -= 2; }
    if (len == 1) sum += *(uint8_t*)addr;
    while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);
    return ~sum;
}

int build_udp_packet(struct sockaddr_in *src, struct sockaddr_in *dst,
                     const char *payload, int plen, uint8_t *buf, int *buflen) {
    struct iphdr *iph = (struct iphdr *)buf;
    struct udphdr *udph = (struct udphdr *)(buf + sizeof(struct iphdr));
    char *data = (char *)(buf + sizeof(struct iphdr) + sizeof(struct udphdr));

    memset(iph, 0, sizeof(struct iphdr));
    iph->version = 4;
    iph->ihl = 5;
    iph->ttl = 64;
    iph->protocol = IPPROTO_UDP;
    iph->saddr = src->sin_addr.s_addr;
    iph->daddr = dst->sin_addr.s_addr;
    iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + plen);
    iph->check = calc_checksum((uint16_t*)iph, sizeof(struct iphdr));

    memset(udph, 0, sizeof(struct udphdr));
    udph->source = src->sin_port;
    udph->dest = dst->sin_port;
    udph->len = htons(sizeof(struct udphdr) + plen);
    memcpy(data, payload, plen);

    struct pseudo_header {
        uint32_t src_ip, dst_ip;
        uint8_t zero, protocol;
        uint16_t udp_len;
    } __attribute__((packed)) psh = {iph->saddr, iph->daddr, 0, IPPROTO_UDP, udph->len};

    size_t pseudo_len = sizeof(psh) + sizeof(struct udphdr) + plen;
    uint8_t pseudo_buf[pseudo_len];
    memcpy(pseudo_buf, &psh, sizeof(psh));
    memcpy(pseudo_buf + sizeof(psh), udph, sizeof(struct udphdr) + plen);
    udph->check = calc_checksum((uint16_t*)pseudo_buf, pseudo_len);
    if (udph->check == 0) udph->check = 0xFFFF;

    *buflen = sizeof(struct iphdr) + sizeof(struct udphdr) + plen;
    return 0;
}

int parse_udp_packet(uint8_t *buf, int len, struct sockaddr_in *src, 
                     struct sockaddr_in *dst, char *payload, int *plen) {
    if ((size_t)len < sizeof(struct iphdr) + sizeof(struct udphdr)) return -1;
    struct iphdr *iph = (struct iphdr *)buf;
    if (iph->protocol != IPPROTO_UDP) return -1;

    struct udphdr *udph = (struct udphdr *)(buf + iph->ihl * 4);
    int data_len = ntohs(udph->len) - sizeof(struct udphdr);
    if (data_len < 0 || data_len > PAYLOAD_BUF) return -1;

    if (src) {
        src->sin_family = AF_INET;
        src->sin_port = udph->source;
        src->sin_addr.s_addr = iph->saddr;
    }
    if (dst) {
        dst->sin_family = AF_INET;
        dst->sin_port = udph->dest;
        dst->sin_addr.s_addr = iph->daddr;
    }
    memcpy(payload, buf + iph->ihl * 4 + sizeof(struct udphdr), data_len);
    *plen = data_len;
    return 0;
}
