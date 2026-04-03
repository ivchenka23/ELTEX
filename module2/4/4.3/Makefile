CC = gcc
CFLAGS = -Wall -std=c11

OBJ = menu.o contacts.o

all: contacts

contacts: $(OBJ)
	$(CC) $(CFLAGS) -o contacts $(OBJ)

menu.o: menu.c contacts.h
	$(CC) $(CFLAGS) -c menu.c -o menu.o

contacts.o: contacts.c contacts.h
	$(CC) $(CFLAGS) -c contacts.c -o contacts.o

clean:
	rm -f *.o contacts

.PHONY: all clean
