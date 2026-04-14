CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
LDFLAGS = -lpthread
TARGET = prod_cons

all: $(TARGET)

$(TARGET): main.c semaphore.c semaphore.h
	$(CC) $(CFLAGS) -o $@ main.c semaphore.c $(LDFLAGS)

clean:
	rm -f $(TARGET) data.txt

.PHONY: all clean

