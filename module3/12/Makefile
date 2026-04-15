CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
TARGET  = chat

.PHONY: all clean

all: $(TARGET)

$(TARGET): chat.c udp_chat.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
