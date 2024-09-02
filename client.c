// client.c
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE 2000

void upload_file(int sock, const char *file_path);
void download_file(int sock, const char *file_name);

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

        // Check for exit command
        if (strcmp(message, "exit") == 0) {
            break;
        }

        // Handle upload command
        if (strncmp(message, "$UPLOAD$", 8) == 0) {
            char *file_path = message + 8; // Extract file path
            upload_file(sock, file_path);
            continue;
        }

        // Handle download command
        if (strncmp(message, "$DOWNLOAD$", 10) == 0) {
            char *file_name = message + 10; // Extract file name
            download_file(sock, file_name);
            continue;
        }

        // Send view command to the server
        if (strcmp(message, "$VIEW$") == 0) {
            if (send(sock, message, strlen(message), 0) < 0) {
                puts("Send failed");
                continue;
            }

            // Receive and display the server response
            int recv_size = recv(sock, server_reply, BUF_SIZE, 0);
            if (recv_size < 0) {
                puts("Recv failed");
            } else {
                server_reply[recv_size] = '\0';
                puts("Files in the server:");
                puts(server_reply);
            }
            continue;
        }

        // Send message to server
        if (send(sock, message, strlen(message), 0) < 0) {
            puts("Send failed");
            return 1;
        }

        // Receive response from server
        int recv_size = recv(sock, server_reply, BUF_SIZE, 0);
        if (recv_size < 0) {
            puts("Recv failed");
            break;
        }
        server_reply[recv_size] = '\0'; // Null-terminate the response

        puts("Server reply:");
        puts(server_reply);
    }

    // Close the socket
    close(sock);
    return 0;
}

// Function to upload a file
void upload_file(int sock, const char *file_path) {
    // Open the file in binary read mode
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Failed to open file"); // Prints the specific reason why fopen failed
        const char *msg = "Failed to open file";
        send(sock, msg, strlen(msg), 0);
        return;
    }

    // Notify the server about the upload
    char upload_command[BUF_SIZE];
    snprintf(upload_command, sizeof(upload_command), "$UPLOAD$%s", file_path);
    send(sock, upload_command, strlen(upload_command), 0);

    // Wait for server confirmation
    char server_reply[BUF_SIZE];
    int recv_size = recv(sock, server_reply, BUF_SIZE, 0);
    if (recv_size < 0 || strncmp(server_reply, "$SUCCESS$", 9) != 0) {
        puts("Upload request failed.");
        fclose(file);
        return;
    }

    // Send the file data in chunks
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

// Function to download a file and save it to the default Downloads folder
void download_file(int sock, const char *file_name) {
    // Define the download path in the default Downloads folder
    char download_path[BUF_SIZE];
    snprintf(download_path, sizeof(download_path), "%s/Downloads/%s", getenv("HOME"), file_name);

    // Open file for writing in the Downloads folder
    FILE *file = fopen(download_path, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    // Notify server of download request
    char download_command[BUF_SIZE];
    snprintf(download_command, sizeof(download_command), "$DOWNLOAD$%s", file_name);
    send(sock, download_command, strlen(download_command), 0);

    // Receive file data from server
    char buffer[BUF_SIZE];
    int bytes_received;
    while ((bytes_received = recv(sock, buffer, BUF_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
        if (bytes_received < BUF_SIZE) {
            break; // End of file
        }
    }

    fclose(file);
    printf("File downloaded successfully to: %s\n", download_path);
}
