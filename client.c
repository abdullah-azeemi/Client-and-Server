// client.c
#include <stdio.h>     // printf, scanf
#include <string.h>    // strlen, strcmp
#include <sys/socket.h> // socket, connect
#include <arpa/inet.h> // inet_addr, sockaddr_in
#include <unistd.h>    // close
#include <stdlib.h>    // exit

#define BUF_SIZE 2000

void upload_file(int sock, const char *file_path);

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char message[BUF_SIZE], server_reply[BUF_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
        return 1;
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8889);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed. Error");
        return 1;
    }
    puts("Connected");

    // Keep communicating with server
    while (1) {
        printf("Enter message (type 'exit' to quit): ");
        scanf("%s", message);

        // Check for exit command
        if (strcmp(message, "exit") == 0) {
            break;
        }

        // Handle upload command
        if (strncmp(message, "$upload$", strlen("$upload$")) == 0) {
            char *file_path = message + strlen("$upload$");
            upload_file(sock, file_path);
            continue;
        }

        // Send the message
        if (send(sock, message, strlen(message), 0) < 0) {
            puts("Send failed");
            return 1;
        }

        // Receive a reply from the server
        int recv_size = recv(sock, server_reply, BUF_SIZE, 0);
        if (recv_size < 0) {
            puts("Recv failed");
            break;
        }
        server_reply[recv_size] = '\0'; // Null-terminate the received string

        puts("Server reply:");
        puts(server_reply);
    }

    // Close the socket
    close(sock);
    return 0;
}

// Function to handle file upload
void upload_file(int sock, const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Failed to open file");
        send(sock, "Failed to open file", strlen("Failed to open file"), 0);
        return;
    }

    // Notify the server about the upload
    char upload_command[BUF_SIZE];
    snprintf(upload_command, sizeof(upload_command), "$upload$%s", file_path);
    send(sock, upload_command, strlen(upload_command), 0);

    // Read and send the file in chunks
    char buffer[BUF_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(sock, buffer, bytes_read, 0) < 0) {
            puts("Send failed");
            break;
        }
    }

    fclose(file);
    puts("File upload completed");
}
