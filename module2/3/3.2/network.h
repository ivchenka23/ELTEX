#ifndef NETWORK_H
#define NETWORK_H
#include <stdint.h>
uint32_t ip_to_int(const char *str);
void int_to_ip(uint32_t ip, char *str);
void process_packets(uint32_t gateway, uint32_t mask, int n);
#endif
