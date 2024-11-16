CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJ = lib/main.o lib/huffman.o

all: genZip

genZip: $(OBJ)
	$(CC) $(CFLAGS) -o genZip $(OBJ)

lib/main.o: lib/main.c lib/huffman.h
	$(CC) $(CFLAGS) -c lib/main.c -o lib/main.o

lib/huffman.o: lib/huffman.c lib/huffman.h
	$(CC) $(CFLAGS) -c lib/huffman.c -o lib/huffman.o

clean:
	rm -f lib/*.o genZip
