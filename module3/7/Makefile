CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lrt

all: ping pong

ping: ping.c pingpong.h
	$(CC) $(CFLAGS) -o $@ ping.c $(LDFLAGS)

pong: pong.c pingpong.h
	$(CC) $(CFLAGS) -o $@ pong.c $(LDFLAGS)

clean:
	rm -f ping pong

.PHONY: all clean

