# Makefile
CC = gcc
CFLAGS = -Wall -pthread

# Targets
all: server client

server: server.c huffman.c server-config.c server-config.h
	$(CC) $(CFLAGS) -o server server.c huffman.c server-config.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f server client
