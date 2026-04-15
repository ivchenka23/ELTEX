#ifndef SNIFFER_H
#define SNIFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <ctype.h>

#define BUF_SIZE 65536

void print_hex_ascii(const unsigned char *data, size_t len);

#endif
