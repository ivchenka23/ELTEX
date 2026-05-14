CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
LDFLAGS = -lpthread
TARGET = taxi_cli

all: $(TARGET)

$(TARGET): main.c shared.h
	$(CC) $(CFLAGS) -o $@ main.c $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
