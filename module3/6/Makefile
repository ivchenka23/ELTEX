CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
LDFLAGS = -lpthread

all: server client

server: server.c chat.h
	$(CC) $(CFLAGS) -o $@ server.c

client: client.c chat.h
	$(CC) $(CFLAGS) -o $@ client.c $(LDFLAGS)

clean:
	rm -f server client

.PHONY: all clean

