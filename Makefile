# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Define the targets and source files
SERVER_TARGET = server
CLIENT_TARGET = client

# Rule to build the server
$(SERVER_TARGET): server.c
	$(CC) $(CFLAGS) -o $(SERVER_TARGET) server.c

# Rule to build the client
$(CLIENT_TARGET): client.c
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) client.c

# Clean rule to remove the compiled files
clean:
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET)
