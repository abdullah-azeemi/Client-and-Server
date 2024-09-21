#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>

#include "server-config.c"  // Include server config for settings

#define BUF_SIZE 2000
#define CRED_FILE "credentials.txt"
#define UPLOAD_DIR "./uploads/"

void *connection_handler(void *client_sock);
char *extract_file_path(const char *input);
void handle_upload(int client_sock, const char *file_path, const char *client_name);
void handle_view(int client_sock, const char *client_name);
void handle_download(int client_sock, const char *file_name, const char *client_name);
int authenticate_client(int client_sock, char *client_name);
void ensure_client_dir(const char *client_name);

int main(int argc, char *argv[]) {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    // Display server configuration
    //display_server_config();

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
        return 1;
    }
    puts("Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    // server.sin_port = htons(get_server_port());
    server.sin_port = htons(8889);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed. Error");
        return 1;
    }
    puts("Bind done");

    listen(socket_desc, 3);
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    // Accept and handle clients in threads
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
        puts("Connection accepted");

        pthread_t client_thread;
        int *new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&client_thread, NULL, connection_handler, (void*)new_sock) < 0) {
            perror("Could not create thread");
            return 1;
        }
        puts("Handler assigned");
    }

    if (client_sock < 0) {
        perror("Accept failed");
        return 1;
    }

    return 0;
}

void *connection_handler(void *client_sock_ptr) {
    int client_sock = *(int*)client_sock_ptr;
    char client_name[BUF_SIZE];

    // Authenticate client
    if (!authenticate_client(client_sock, client_name)) {
        close(client_sock);
        free(client_sock_ptr);
        return NULL;
    }

    // Ensure client directory exists
    ensure_client_dir(client_name);

    int read_size;
    char client_message[BUF_SIZE];

    // Receive a message from client
    while ((read_size = recv(client_sock, client_message, BUF_SIZE, 0)) > 0) {
        client_message[read_size] = '\0';

        if (strncmp(client_message, "$upload$", 8) == 0) {
            char *file_path = extract_file_path(client_message);
            if (file_path) {
                handle_upload(client_sock, file_path, client_name);
                free(file_path);
            } else {
                write(client_sock, "Invalid upload command", 22);
            }
        } else if (strncmp(client_message, "$view$", 6) == 0) {
            handle_view(client_sock, client_name);
        } else if (strncmp(client_message, "$download$", 10) == 0) {
            char *file_name = extract_file_path(client_message);
            if (file_name) {
                handle_download(client_sock, file_name, client_name);
                free(file_name);
            } else {
                write(client_sock, "Invalid download command", 25);
            }
        } else {
            write(client_sock, "Unknown command", 15);
        }
    }

    if (read_size == 0) {
        puts("Client disconnected");
    } else if (read_size == -1) {
        perror("recv failed");
    }

    close(client_sock);
    free(client_sock_ptr);
    return NULL;
}

// Authenticate client using credentials stored in a file
int authenticate_client(int client_sock, char *client_name) {
    char received_credentials[BUF_SIZE];
    int read_size = recv(client_sock, received_credentials, BUF_SIZE, 0);
    if (read_size <= 0) {
        return 0;
    }

    FILE *cred_file = fopen(CRED_FILE, "r");
    if (!cred_file) {
        perror("Could not open credentials file");
        return 0;
    }

    char file_line[BUF_SIZE];
    while (fgets(file_line, sizeof(file_line), cred_file)) {
        file_line[strcspn(file_line, "\n")] = 0;  // Remove newline
        if (strcmp(received_credentials, file_line) == 0) {
            // Extract client name from credentials (username:password)
            sscanf(received_credentials, "%[^:]", client_name);
            fclose(cred_file);
            write(client_sock, "Authentication Successful", 25);
            return 1;
        }
    }

    fclose(cred_file);
    write(client_sock, "Authentication Failed", 21);
    return 0;
}

void ensure_client_dir(const char *client_name) {
    char client_dir[BUF_SIZE];
    snprintf(client_dir, sizeof(client_dir), "%s%s", UPLOAD_DIR, client_name);

    struct stat st = {0};
    if (stat(client_dir, &st) == -1) {
        mkdir(client_dir, 0700);  // Create directory for client if it doesn't exist
    }
}

// Handle file upload (store encoded file)
void handle_upload(int client_sock, const char *file_path, const char *client_name) {
    char full_path[BUF_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s/%s", UPLOAD_DIR, client_name, file_path);

    FILE *file = fopen(full_path, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        write(client_sock, "Failed to open file for writing", 31);
        return;
    }

    char buffer[BUF_SIZE];
    int bytes_received;

    // Save encoded file directly
    while ((bytes_received = recv(client_sock, buffer, BUF_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
        if (bytes_received < BUF_SIZE) {
            break;
        }
    }

    fclose(file);
    write(client_sock, "File uploaded and saved in encoded format", 41);
}

// Handle viewing files in client directory
void handle_view(int client_sock, const char *client_name) {
    char client_dir[BUF_SIZE];
    snprintf(client_dir, sizeof(client_dir), "%s%s", UPLOAD_DIR, client_name);

    DIR *d;
    struct dirent *dir;
    d = opendir(client_dir);

    if (!d) {
        write(client_sock, "Failed to open client directory", 30);
        return;
    }

    char file_list[BUF_SIZE] = "Files:\n";
    char file_info[BUF_SIZE];
    struct stat file_stat;
    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            snprintf(file_info, sizeof(file_info), "%s/%s", client_dir, dir->d_name);
            stat(file_info, &file_stat);
            snprintf(file_info, sizeof(file_info), "- %s (%ld bytes)\n", dir->d_name, file_stat.st_size);
            strcat(file_list, file_info);
        }
    }
    closedir(d);
    write(client_sock, file_list, strlen(file_list));
}

// Handle file download in encoded format
void handle_download(int client_sock, const char *file_name, const char *client_name) {
    char full_path[BUF_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s/%s", UPLOAD_DIR, client_name, file_name);

    FILE *file = fopen(full_path, "rb");
    if (!file) {
        write(client_sock, "File not found", 14);
        return;
    }

    char buffer[BUF_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(client_sock, buffer, bytes_read, 0);
    }

    fclose(file);
    write(client_sock, "File download complete", 22);
}

char* extract_file_path(const char *input) {
    const char *file_path = input + strlen("$upload$");
    char *end = strchr(file_path, '$');
    if (end) {
        size_t path_len = end - file_path;
        char *extracted_path = (char*)malloc(path_len + 1);
        strncpy(extracted_path, file_path, path_len);
        extracted_path[path_len] = '\0';
        return extracted_path;
    }
    return NULL;
}
