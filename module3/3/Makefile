CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = contacts_app

SRCS = menu.c contacts.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) contacts.dat

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

