CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
TARGETS = server client

all: $(TARGETS)

server: server.c common.c common.h
	$(CC) $(CFLAGS) -o $@ server.c common.c

client: client.c common.c common.h
	$(CC) $(CFLAGS) -o $@ client.c common.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
