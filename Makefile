# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Define the targets and source files
SERVER_TARGET = server
CLIENT_TARGET = client
SOURCES = server.c client.c cJSON.c

# Rule to build the server
$(SERVER_TARGET): server.c cJSON.c
    $(CC) $(CFLAGS) -o $(SERVER_TARGET) server.c cJSON.c

# Rule to build the client
$(CLIENT_TARGET): client.c cJSON.c
    $(CC) $(CFLAGS) -o $(CLIENT_TARGET) client.c cJSON.c

# Clean rule to remove the compiled files
clean:
    rm -f $(SERVER_TARGET) $(CLIENT_TARGET)
