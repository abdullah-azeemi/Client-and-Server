#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include "server-config.h" 
#include "huffman.h"     

#define BUF_SIZE 2000
#define UPLOAD_DIR "./uploads/"
#define CLIENT_STORAGE_LIMIT 10240 // 10 KB

// Function prototypes
void *handle_client_thread(void *client_sock_ptr);
void handle_upload(int client_sock, const char *file_path);
void handle_view(int client_sock);
void handle_download(int client_sock, const char *file_name);
int check_storage_space();
void create_upload_dir();
void encrypt_file(const char *input_path, const char *output_path);
void decrypt_file(const char *input_path, const char *output_path);

int main() {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;
    pthread_t thread_id;

    // Load server configuration
    load_configuration();
    print_configuration();

    // Create upload directory if it doesn't exist
    create_upload_dir();

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
        return 1;
    }
    puts("Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8889);

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed. Error");
        return 1;
    }
    puts("Bind done");

    // Listen
    listen(socket_desc, max_clients);
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    // Accept and handle connections in separate threads
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))) {
        printf("Connection accepted\n");

        if (pthread_create(&thread_id, NULL, handle_client_thread, (void *)&client_sock) < 0) {
            perror("Could not create thread");
            return 1;
        }

        puts("Handler assigned");
    }

    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }

    close(socket_desc);
    return 0;
}

void *handle_client_thread(void *client_sock_ptr) {
    int client_sock = *(int *)client_sock_ptr;
    char client_message[BUF_SIZE];
    int read_size;

    while ((read_size = recv(client_sock, client_message, BUF_SIZE, 0)) > 0) {
        client_message[read_size] = '\0';

        if (strncmp(client_message, "$UPLOAD$", 8) == 0) {
            char *file_path = client_message + 8;
            handle_upload(client_sock, file_path);
        } else if (strncmp(client_message, "$VIEW$", 6) == 0) {
            handle_view(client_sock);
        } else if (strncmp(client_message, "$DOWNLOAD$", 10) == 0) {
            char *file_name = client_message + 10;
            handle_download(client_sock, file_name);
        } else {
            const char *msg = "Unknown Command";
            write(client_sock, msg, strlen(msg));
        }
    }

    if (read_size == 0) {
        puts("Client disconnected");
    } else if (read_size == -1) {
        perror("recv failed");
    }

    close(client_sock);
    return 0;
}

void handle_upload(int client_sock, const char *file_path) {
    if (!check_storage_space()) {
        const char *msg = "$FAILURE$LOW_SPACE$";
        write(client_sock, msg, strlen(msg));
        return;
    }

    char full_path[BUF_SIZE];
    snprintf(full_path, BUF_SIZE, "%s%s", UPLOAD_DIR, file_path);

    // Temporary file to save the uploaded data before encryption
    char temp_path[BUF_SIZE];
    if (snprintf(temp_path, sizeof(temp_path), "%s.temp", full_path) >= sizeof(temp_path)) {
        perror("File path too long to add .temp suffix");
        const char *msg = "File path too long";
        write(client_sock, msg, strlen(msg));
        return;
    }

    FILE *fp = fopen(temp_path, "wb");
    if (!fp) {
        perror("Failed to open file for writing");
        const char *msg = "Failed to open file for writing";
        write(client_sock, msg, strlen(msg));
        return;
    }

    const char *msg = "$SUCCESS$";
    write(client_sock, msg, strlen(msg));

    char buffer[BUF_SIZE];
    int bytes_rcvd;
    while ((bytes_rcvd = recv(client_sock, buffer, BUF_SIZE, 0)) > 0) {
        fwrite(buffer, sizeof(char), bytes_rcvd, fp);
        if (bytes_rcvd < BUF_SIZE) {
            break;
        }
    }

    fclose(fp);

    if (encryption_algorithm == ENCRYPTION_ALGO_HUFFMAN) {
        encrypt_file(temp_path, full_path);
        remove(temp_path); 
    } else {
        rename(temp_path, full_path);
    }

    const char *success_msg = "$SUCCESS$";
    write(client_sock, success_msg, strlen(success_msg));
}


void handle_view(int client_sock) {
    DIR *d = opendir(UPLOAD_DIR);
    struct dirent *dir;
    struct stat file_stat;
    char file_info[BUF_SIZE] = "";

    if (!d) {
        const char *msg = "$FAILURE$NO_CLIENT_DATA$";
        write(client_sock, msg, strlen(msg));
        return;
    }

    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            char file_path[BUF_SIZE];
            snprintf(file_path, sizeof(file_path), "%s%s", UPLOAD_DIR, dir->d_name);
            if (stat(file_path, &file_stat) == 0) {
                char temp[BUF_SIZE];
                snprintf(temp, sizeof(temp), "Name: %s, Size: %ld bytes\n", dir->d_name, file_stat.st_size);
                strcat(file_info, temp);
            }
        }
    }

    closedir(d);

    if (strlen(file_info) == 0) {
        const char *msg = "$FAILURE$NO_CLIENT_DATA$";
        write(client_sock, msg, strlen(msg));
    } else {
        write(client_sock, file_info, strlen(file_info));
    }
}

void handle_download(int client_sock, const char *file_name) {
    char full_path[BUF_SIZE];
    snprintf(full_path, BUF_SIZE, "%s%s", UPLOAD_DIR, file_name);

    char temp_path[BUF_SIZE];

    if (snprintf(temp_path, sizeof(temp_path), "%s.temp", full_path) >= sizeof(temp_path)) {
        perror("File path too long to add .temp suffix");
        const char *msg = "File path too long";
        write(client_sock, msg, strlen(msg));
        return;
    }

    if (encryption_algorithm == ENCRYPTION_ALGO_HUFFMAN) {
        decrypt_file(full_path, temp_path);
        strcpy(full_path, temp_path); 
    }

    FILE *fp = fopen(full_path, "rb");
    if (!fp) {
        const char *msg = "$FAILURE$FILE_NOT_FOUND$";
        write(client_sock, msg, strlen(msg));
        return;
    }

    char buffer[BUF_SIZE];
    int bytes_sent;
    while ((bytes_sent = fread(buffer, sizeof(char), BUF_SIZE, fp)) > 0) {
        send(client_sock, buffer, bytes_sent, 0);
    }

    fclose(fp);
    if (encryption_algorithm == ENCRYPTION_ALGO_HUFFMAN) {
        remove(temp_path);
    }
}

int check_storage_space() {
    struct statvfs stat;
    if (statvfs(UPLOAD_DIR, &stat) != 0) {
        return 0;
    }
    unsigned long available_space = stat.f_bsize * stat.f_bavail;
    return available_space >= CLIENT_STORAGE_LIMIT;
}

void create_upload_dir() {
    struct stat st = {0};
    if (stat(UPLOAD_DIR, &st) == -1) {
        mkdir(UPLOAD_DIR, 0700);
    }
}

void encrypt_file(const char *input_path, const char *output_path) {
    FILE *in = fopen(input_path, "rb");
    FILE *out = fopen(output_path, "wb");
    if (!in || !out) {
        perror("File open failed");
        return;
    }
    int freq[256] = {0};
    char data[256];
    int size = 0;

    int ch;
    while ((ch = fgetc(in)) != EOF) {
        if (freq[ch] == 0) {
            data[size++] = ch;
        }
        freq[ch]++;
    }

    
    HuffmanCodes(data, freq, size);

    fclose(in);
    fclose(out);
}

// Huffman decryption function
void decrypt_file(const char *input_path, const char *output_path) {
    
    printf("Decrypting file: %s -> %s\n", input_path, output_path);

    FILE *in = fopen(input_path, "rb");
    FILE *out = fopen(output_path, "wb");
    if (!in || !out) {
        perror("File open failed");
        return;
    }

    int ch;
    while ((ch = fgetc(in)) != EOF) {
        fputc(ch, out);
    }

    fclose(in);
    fclose(out);
}
