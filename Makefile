OBJ0 = test.o pq.o cli.o
OBJ1 = sankaque.o $(OBJ0)
OBJ2 = sankaque-cli.o $(OBJ0)

LDFLAGS = -lm -lpthread
CC=gcc
CFLAGS=-g -Wall

default: sankaque sankaque-cli

sankaque: $(OBJ1)
	$(CC) -o $@ $^ $(LDFLAGS)

sankaque-cli: $(OBJ2)
	$(CC) -o $@ $^ $(LDFLAGS) 

clean:
	-rm -f *.o
	-rm -f sankaque sankaque-cli

push:
	-rsync -av . root@hexafoil:/root/sankaque

pull:
	-rsync -av root@hexafoil:/root/sankaque/ .
