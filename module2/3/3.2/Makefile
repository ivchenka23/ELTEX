CC = gcc
CFLAGS = -Wall -std=c11

OBJ = menu.o network.o

all: router_sim

router_sim: $(OBJ)
	$(CC) $(CFLAGS) -o router_sim $(OBJ)

main.o: main.c network.h
	$(CC) $(CFLAGS) -c menu.c -o menu.o

network.o: network.c network.h
	$(CC) $(CFLAGS) -c network.c -o network.o

clean:
	rm -f *.o router_sim

.PHONY: all clean
