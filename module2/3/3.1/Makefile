CC = gcc
CFLAGS = -Wall -std=c11

OBJ = menu.o permissions.o

all: permissions

permissions: $(OBJ)
	$(CC) $(CFLAGS) -o permissions $(OBJ)

menu.o: menu.c permissions.h
	$(CC) $(CFLAGS) -c menu.c -o menu.o

permissions.o: permissions.c permissions.h
	$(CC) $(CFLAGS) -c permissions.c -o permissions.o

clean:
	rm -f *.o permissions

.PHONY: all clean
