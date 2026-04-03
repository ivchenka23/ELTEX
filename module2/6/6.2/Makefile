CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -fPIC

LIB_NAME = libcontacts.so
EXEC_NAME = contacts_app

all: $(EXEC_NAME)

$(LIB_NAME): contacts.o
	$(CC) -shared -o $@ $^

$(EXEC_NAME): menu.o $(LIB_NAME)
	$(CC) $(CFLAGS) menu.o -Wl,-rpath,'$$ORIGIN' -L. -lcontacts -o $(EXEC_NAME)

menu.o: menu.c contacts.h
	$(CC) $(CFLAGS) -c menu.c -o $@

contacts.o: contacts.c contacts.h
	$(CC) $(CFLAGS) -c contacts.c -o $@

clean:
	rm -f *.o $(LIB_NAME) $(EXEC_NAME)

.PHONY: all clean
