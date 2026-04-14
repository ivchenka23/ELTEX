CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = task01

SRCS = main.c process.c utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	$(TARGET) 42 hello 3.14 world 100

.PHONY: all clean run

