#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "network.h"

uint32_t ip_to_int(const char *str) {
    unsigned int a, b, c, d;
    if (sscanf(str, "%u.%u.%u.%u", &a, &b, &c, &d) != 4) {
        fprintf(stderr, "Ошибка: Неверный формат IP\n");
        exit(1);
    }
    return (a << 24) | (b << 16) | (c << 8) | d;
}

void int_to_ip(uint32_t ip, char *str) {
    snprintf(str, 16, "%u.%u.%u.%u",
             (ip >> 24) & 0xFF,
             (ip >> 16) & 0xFF,
             (ip >> 8) & 0xFF,
             ip & 0xFF);
}

void process_packets(uint32_t gateway, uint32_t mask, int n) {
    uint32_t local_network = gateway & mask;
    int local_cnt = 0;
    int remote_cnt = 0;
    char ip_str[16];

    srand(time(NULL));

    for (int i = 0; i < n; i++) {
        uint32_t dest_ip = ((uint32_t)(rand() % 256) << 24) |
                           ((uint32_t)(rand() % 256) << 16) |
                           ((uint32_t)(rand() % 256) << 8) |
                           (uint32_t)(rand() % 256);

        if ((dest_ip & mask) == local_network) {
            local_cnt++;
        } else {
            remote_cnt++;
        }
    }

    double local_pct = (double)local_cnt / n * 100.0;
    double remote_pct = (double)remote_cnt / n * 100.0;

    printf("Сеть шлюза: ");
    int_to_ip(local_network, ip_str);
    printf("%s\n", ip_str);
    printf("Своя подсеть: %d пакетов (%.2f%%)\n", local_cnt, local_pct);
    printf("Другие сети:  %d пакетов (%.2f%%)\n", remote_cnt, remote_pct);
}
