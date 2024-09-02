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

#define BUF_SIZE 2000
#define UPLOAD_DIR "./uploads/"
#define CLIENT_STORAGE_LIMIT 10240 // 10kb

void handle_client(int client_sock);
void handle_upload(int client_sock, const char *file_path);
void handle_view(int client_sock);
void handle_download(int client_sock, const char *file_name);
int check_storage_space();
void create_upload_dir();

int main() {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

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
    listen(socket_desc, 3);
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    // Accept connections
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    handle_client(client_sock);

    close(client_sock);
    close(socket_desc);

    return 0;
}

void handle_client(int client_sock) {
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
}

void handle_upload(int client_sock, const char *file_path) {
    if (!check_storage_space()) {
        const char *msg = "$FAILURE$LOW_SPACE$";
        write(client_sock, msg, strlen(msg));
        return;
    }

    char full_path[BUF_SIZE];
    snprintf(full_path, BUF_SIZE, "%s%s", UPLOAD_DIR, file_path);

    FILE *fp = fopen(full_path, "wb");
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

    // iterating through files in Directory
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

    // Send file information to the client
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
