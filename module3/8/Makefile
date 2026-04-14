CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
TARGETS = producer consumer

all: $(TARGETS)

producer: producer.c semaphore.c semaphore.h
	$(CC) $(CFLAGS) -o $@ producer.c semaphore.c

consumer: consumer.c semaphore.c semaphore.h
	$(CC) $(CFLAGS) -o $@ consumer.c semaphore.c

clean:
	rm -f $(TARGETS) data*.txt

.PHONY: all clean

