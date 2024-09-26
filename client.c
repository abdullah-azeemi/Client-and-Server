#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE 2000

void upload_file(int sock, const char *file_path);
void view_files(int sock);
void download_file(int sock, const char *file_name);
char *rle_encode(const char *data, size_t length);
char *rle_decode(const char *data, size_t length);

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

    // Authentication (Username:Password)
    char credentials[BUF_SIZE];
    printf("Enter username:password: ");
    scanf("%s", credentials);
    send(sock, credentials, strlen(credentials), 0);

    // Waiting for auth response
    int recv_size = recv(sock, server_reply, BUF_SIZE, 0);
    if (recv_size < 0) {
        puts("Recv failed");
        return 1;
    }
    server_reply[recv_size] = '\0';
    printf("Server reply: %s\n", server_reply);

    if (strcmp(server_reply, "Authentication Failed") == 0 || strcmp(server_reply, "Client already logged in") == 0) {
        puts("Authentication failed. Exiting...");
        close(sock);
        return 1;
    }

    while (1) {
        printf("Enter command ($upload$, $view$, $download$, or 'exit'): ");
        scanf("%s", message);

        if (strcmp(message, "exit") == 0) {
            break;
        }

        if (strncmp(message, "$upload$", 8) == 0) {
            char *file_path = message + strlen("$upload$");
            upload_file(sock, file_path);
        } else if (strcmp(message, "$view$") == 0) {
            view_files(sock);
        } else if (strncmp(message, "$download$", 10) == 0) {
            char *file_name = message + strlen("$download$");
            download_file(sock, file_name);
        } else {
            if (send(sock, message, strlen(message), 0) < 0) {
                puts("Send failed");
                return 1;
            }

            recv_size = recv(sock, server_reply, BUF_SIZE, 0);
            if (recv_size < 0) {
                puts("Recv failed");
                break;
            }
            server_reply[recv_size] = '\0';
            printf("Server reply: %s\n", server_reply);
        }
    }

    close(sock);
    return 0;
}

void upload_file(int sock, const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Failed to open file");
        send(sock, "Failed to open file", strlen("Failed to open file"), 0);
        return;
    }

    char upload_command[BUF_SIZE];
    snprintf(upload_command, sizeof(upload_command), "$upload$%s$", file_path);
    send(sock, upload_command, strlen(upload_command), 0);

    char buffer[BUF_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        char *encoded_data = rle_encode(buffer, bytes_read);
        if (send(sock, encoded_data, strlen(encoded_data), 0) < 0) {
            puts("Send failed");
            free(encoded_data);
            break;
        }
        free(encoded_data);
    }

    fclose(file);
    puts("File upload (encoded) completed");
}

void view_files(int sock) {
    send(sock, "$view$", strlen("$view$"), 0);

    char server_reply[BUF_SIZE];
    int recv_size = recv(sock, server_reply, BUF_SIZE, 0);
    if (recv_size > 0) {
        server_reply[recv_size] = '\0';
        printf("Server reply: %s\n", server_reply);
    } else {
        puts("Failed to retrieve file list");
    }
}

void download_file(int sock, const char *file_name) {
    char download_command[BUF_SIZE];
    const char *client_name = "client1";
    snprintf(download_command, sizeof(download_command), "uploads/%s/%s",client_name, file_name);
    send(sock, download_command, strlen(download_command), 0);

    char buffer[BUF_SIZE];
    int bytes_received;
    char new_file_name[BUF_SIZE];
    strcpy(new_file_name, file_name);

    // Check for existing file
    int version = 1;
    while (access(new_file_name, F_OK) == 0) {
        snprintf(new_file_name, BUF_SIZE, "%s(%d)", file_name, version++);
    }

    FILE *file = fopen(new_file_name, "wb");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    while ((bytes_received = recv(sock, buffer, BUF_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
    printf("File download completed: %s\n", new_file_name);
}

char *rle_encode(const char *data, size_t length) {
    char *encoded = (char *)malloc(length * 2 + 1);
    if (!encoded) {
        return NULL;
    }

    int count;
    char current;
    size_t encoded_index = 0;

    for (size_t i = 0; i < length; i++) {
        current = data[i];
        count = 1;

        while (i + 1 < length && data[i + 1] == current) {
            count++;
            i++;
        }

        encoded_index += snprintf(encoded + encoded_index, length * 2 + 1 - encoded_index, "%d%c", count, current);
    }
    encoded[encoded_index] = '\0';
    return encoded;
}

char *rle_decode(const char *data, size_t length) {
    char *decoded = (char *)malloc(length * 2 + 1);  // Allocate enough space for decoded data
    if (!decoded) {
        return NULL;
    }

    size_t decoded_index = 0;
    int count;
    char current_char;

    for (size_t i = 0; i < length; i++) {
        count = 0;

        while (i < length && data[i] >= '0' && data[i] <= '9') {
            count = count * 10 + (data[i] - '0');
            i++;
        }
        if (i < length) {
            current_char = data[i];
            for (int j = 0; j < count; j++) {
                decoded[decoded_index++] = current_char;
            }
        }
    }

    decoded[decoded_index] = '\0';
    return decoded;
}
