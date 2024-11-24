CC = gcc
CFLAGS = -Wall -pthread

# Targets
all: server client

server: server.c server-config.c memory_manager.c
	$(CC) $(CFLAGS) -o server server.c server-config.c memory_manager.c

client: client.c memory_manager.c
	$(CC) $(CFLAGS) -o client client.c memory_manager.c

clean:
	rm -f server client
