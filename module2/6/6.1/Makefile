CC = gcc
CFLAGS = -Wall -Wextra -std=c11
AR = ar
ARFLAGS = rcs

LIB_NAME = libcontacts.a
EXEC_NAME = contacts_app

all: $(EXEC_NAME)

$(LIB_NAME): contacts.o
	$(AR) $(ARFLAGS) $@ $^

$(EXEC_NAME): menu.o $(LIB_NAME)
	$(CC) $(CFLAGS) menu.o $(LIB_NAME) -o $(EXEC_NAME)

menu.o: menu.c contacts.h
	$(CC) $(CFLAGS) -c menu.c -o $@

contacts.o: contacts.c contacts.h
	$(CC) $(CFLAGS) -c contacts.c -o $@

clean:
	rm -f *.o $(LIB_NAME) $(EXEC_NAME)

.PHONY: all clean
