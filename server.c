#include <stdio.h>
#include <string.h>   //strlen
#include <sys/socket.h>
#include <arpa/inet.h>   //inet_addr
#include <unistd.h>   //write
#include <cJson/cJson.h>


#define BUF_SIZE 2000
#define UPLOAD_DIR "./uploads/"


int is_command(const char* input);
char * extract_file_path(const char * input);
void handle_upload(int client_sock, const char * file_path);


int main(int argc, char *argv[]) {
    int socket_desc, client_sock, c, read_size;
    struct sockaddr_in server, client;
    char client_message[BUF_SIZE];

    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
        return 1;
    }
    puts("Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8889);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    listen(socket_desc, 3);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    while ((read_size = recv(client_sock, client_message, BUF_SIZE, 0)) > 0) {
        client_message[read_size] = '\0'; 

        char * welcome_msg[] = "aur paen ki hal chal";
        char * unwelcome_msg[] = "bhai ap kon";
        if (strcmp(client_message, "hello") == 0) {
            write(client_sock,welcome_msg,sizeof(welcome_msg));
        } else {
            write(client_sock, unwelcome_msg, size_of(unwelcome_msg));
        }
    }

    if (read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if (read_size == -1) {
        perror("recv failed");
    }

    close(client_sock);
    close(socket_desc);
    return 0;
}

char* extract_file_path(const char * input){
    
    if(!is_command(input)){
        return NULL;
    }   

    const char * file_path = input + strlen("$upload$");
    return strdup(file_path);
}

void handle_upload(int client_sock, const char * file_path){
    char full_path[BUF_SIZE];
    snprintf(full_path, BUF_SIZE, "%s%s", UPLOAD_DIR, file_path);

    FILE * file = fopen(full_path, "wb");
    if (!file){
        perror("fopen failed");
        char * failed_writing_msg [] = "Failed to open file for writing";
        write(client_sock, failed_writing_msg, size_of(failed_writing_msg));
        return;
    }

    char buffer[BUF_SIZE];
    int bytes_received;
    while((bytes_received = recv(client_sock, buffer, BUF_SIZE, 0)) > 0){
        fwrite(buffer, bytes_received, 1, file);
        if (bytes_received < sizeof(buffer)){
            break;
        }
    }
    fclose(file);
    char * successful_upload_msg[] = "File uploaded successfully";
    write(client_sock,successful_upload_msg, size_of(successful_upload_msg));
}
