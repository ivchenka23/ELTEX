CC = gcc
CFLAGS = -Wall -std=c11

OBJ = menu.o calculator.o

all: calculator

calculator: $(OBJ)
	$(CC) $(CFLAGS) -o calculator $(OBJ) -lm

menu.o: menu.c calculator.h
	$(CC) $(CFLAGS) -c menu.c -o menu.o

calculator.o: calculator.c calculator.h
	$(CC) $(CFLAGS) -c calculator.c -o calculator.o

clean:
	rm -f *.o calculator

.PHONY: all clean
