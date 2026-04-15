#define _DEFAULT_SOURCE
#include "sniffer.h"

void print_hex_ascii(const unsigned char *data, size_t len) {
    size_t i, j;
    for (i = 0; i < len; i++) {
        printf("%02x ", data[i]);
        if ((i + 1) % 16 == 0) {
            printf("  ");
            for (j = i - 15; j <= i; j++)
                printf("%c", isprint(data[j]) ? data[j] : '.');
            printf("\n");
        }
    }
    if (len % 16 != 0) {
        for (j = len % 16; j < 16; j++) printf("   ");
        printf("  ");
        for (j = len - (len % 16); j < len; j++)
            printf("%c", isprint(data[j]) ? data[j] : '.');
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port> [interface]\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    const char *interface = (argc > 2) ? argv[2] : "eth0";

    int raw_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_sock < 0) {
        perror("socket");
        return 1;
    }

    struct ifreq if_idx;
    memset(&if_idx, 0, sizeof(if_idx));
    strncpy(if_idx.ifr_name, interface, IF_NAMESIZE - 1);
    if (ioctl(raw_sock, SIOCGIFINDEX, &if_idx) < 0) {
        perror("ioctl");
        close(raw_sock);
        return 1;
    }

    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_idx.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_IP);

    if (bind(raw_sock, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        perror("bind");
        close(raw_sock);
        return 1;
    }

    FILE *dump_file = fopen("capture.dump", "wb");
    if (!dump_file) {
        perror("fopen");
        close(raw_sock);
        return 1;
    }

    unsigned char buffer[BUF_SIZE];
    socklen_t from_len = sizeof(struct sockaddr_ll);
    int packet_count = 0;

    printf("Sniffing on %s for UDP port %d...\n", interface, port);
    printf("Press Ctrl+C to stop\n\n");

    while (1) {
        ssize_t data_size = recvfrom(raw_sock, buffer, BUF_SIZE, 0, NULL, &from_len);
        if (data_size < 0) {
            perror("recvfrom");
            continue;
        }

        if (data_size < 14 + 20 + 8) continue;

        unsigned char *eth = buffer;
        if (eth[12] != 0x08 || eth[13] != 0x00) continue;

        struct iphdr *ip = (struct iphdr *)(buffer + 14);
        if (ip->version != 4) continue;
        if (ip->protocol != IPPROTO_UDP) continue;

        struct udphdr *udp = (struct udphdr *)((unsigned char *)ip + ip->ihl * 4);
        int dest_port = ntohs(udp->dest);
        int src_port = ntohs(udp->source);

        if (dest_port != port && src_port != port) continue;

        packet_count++;
        unsigned char *data = (unsigned char *)udp + sizeof(struct udphdr);
        int data_len = ntohs(udp->len) - sizeof(struct udphdr);

        printf("=== Packet #%d ===\n", packet_count);
        printf("Source: %s:%d\n", inet_ntoa(*(struct in_addr *)&ip->saddr), src_port);
        printf("Dest: %s:%d\n", inet_ntoa(*(struct in_addr *)&ip->daddr), dest_port);
        printf("Data length: %d bytes\n", data_len);
        printf("Payload (hex+ascii):\n");
        print_hex_ascii(data, data_len);
        printf("\n");

        fwrite(&packet_count, sizeof(int), 1, dump_file);
        fwrite(&ip->saddr, sizeof(ip->saddr), 1, dump_file);
        fwrite(&ip->daddr, sizeof(ip->daddr), 1, dump_file);
        fwrite(&src_port, sizeof(int), 1, dump_file);
        fwrite(&dest_port, sizeof(int), 1, dump_file);
        fwrite(&data_len, sizeof(int), 1, dump_file);
        fwrite(data, 1, data_len, dump_file);
        fflush(dump_file);
    }

    fclose(dump_file);
    close(raw_sock);
    return 0;
}
