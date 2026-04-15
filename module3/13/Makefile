CC      = gcc
CFLAGS  = -Wall -Wextra -std=gnu11 -O2
TARGETS = server client

.PHONY: all clean

all: $(TARGETS)

server: server.c tcp_calc.h
	$(CC) $(CFLAGS) -o $@ $<

client: client.c tcp_calc.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
