CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
TARGET = shm_demo

all: $(TARGET)

$(TARGET): main.c shared.h
	$(CC) $(CFLAGS) -o $@ main.c

clean:
	rm -f $(TARGET)

.PHONY: all clean

