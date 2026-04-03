CC = gcc
CFLAGS = -Wall -std=c11

OBJ = menu.o queue.o

all: priority_queue

priority_queue: $(OBJ)
	$(CC) $(CFLAGS) -o priority_queue $(OBJ)

main.o: menu.c queue.h
	$(CC) $(CFLAGS) -c menu.c -o menu.o

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c -o queue.o

clean:
	rm -f *.o priority_queue

.PHONY: all clean
