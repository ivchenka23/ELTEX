CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
TARGET  = sniffer

.PHONY: all clean

all: $(TARGET)

$(TARGET): sniffer.c sniffer.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET) capture.dump
