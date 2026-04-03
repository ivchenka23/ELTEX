#include <stdio.h>
#include <stdlib.h>
#include "network.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Использование: %s <Gateway_IP> <Mask> <N>\n", argv[0]);
        return 1;
    }

    uint32_t gateway = ip_to_int(argv[1]);
    uint32_t mask = ip_to_int(argv[2]);
    int n = atoi(argv[3]);

    if (n <= 0) {
        fprintf(stderr, "Ошибка: количество пакетов должно быть больше 0\n");
        return 1;
    }

    process_packets(gateway, mask, n);
    return 0;
}
