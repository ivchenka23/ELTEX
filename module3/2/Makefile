CC = gcc
CFLAGS = -Wall -Wextra -std=c11

SHELL_SRCS = main.c shell.c
SHELL_OBJS = $(SHELL_SRCS:.c=.o)
SHELL_TARGET = mysh

PROGRAMS = sum concat maxval

all: $(SHELL_TARGET) $(PROGRAMS)

$(SHELL_TARGET): $(SHELL_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

sum: sum.c
	$(CC) $(CFLAGS) -o $@ $^

concat: concat.c
	$(CC) $(CFLAGS) -o $@ $^

maxval: maxval.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(SHELL_OBJS) $(SHELL_TARGET) $(PROGRAMS)

run: $(SHELL_TARGET) $(PROGRAMS)
	./$(SHELL_TARGET)

.PHONY: all clean run

