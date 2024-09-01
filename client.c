// client.c
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

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

    // Communicate with server
    while (1) {
        printf("Enter command: ");
        scanf("%s", message);

        if (strcmp(message, "exit") == 0) {
            break;
        }

        if (strncmp(message, "$UPLOAD$", 8) == 0) {
            char *file_path = message + 8;
            upload_file(sock, file_path);
            continue;
        }

        // Send message
        if (send(sock, message, strlen(message), 0) < 0) {
            puts("Send failed");
            return 1;
        }

        // Receive response
        int recv_size = recv(sock, server_reply, BUF_SIZE, 0);
        if (recv_size < 0) {
            puts("Recv failed");
            break;
        }
        server_reply[recv_size] = '\0'; // Null-terminate the response

        puts("Server reply:");
        puts(server_reply);
    }

    close(sock);
    return 0;
}

void upload_file(int sock, const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Failed to open file");
        const char *msg = "Failed to open file";
        send(sock, msg, strlen(msg), 0);
        return;
    }

    // Notify server about the upload
    char upload_command[BUF_SIZE];
    snprintf(upload_command, sizeof(upload_command), "$UPLOAD$%s", file_path);
    send(sock, upload_command, strlen(upload_command), 0);

    char server_reply[BUF_SIZE];
    int recv_size = recv(sock, server_reply, BUF_SIZE, 0);
    if (recv_size < 0 || strncmp(server_reply, "$SUCCESS$", 9) != 0) {
        puts("Upload request failed.");
        fclose(file);
        return;
    }

    // Send file data
    char buffer[BUF_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(sock, buffer, bytes_read, 0) < 0) {
            puts("Send failed");
            break;
        }
    }

    fclose(file);

    // Receive final server response
    recv_size = recv(sock, server_reply, BUF_SIZE, 0);
    if (recv_size < 0) {
        puts("Recv failed");
    } else {
        server_reply[recv_size] = '\0';
        puts("Server response:");
        puts(server_reply);
    }
}
