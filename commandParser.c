#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

#define COMMAND_UPLOAD "$upload$"
#define COMMAND_DOWNLOAD "$download$"

bool is_command(const char *input){
    return (strncmp(input,COMMAND_UPLOAD, strlen(COMMAND_UPLOAD)) == 0);
}

char * extract_file_path(const char * input){
    if(!is_command(input)){
        return NULL;
    }

    const char * file_path = input + strlen(COMMAND_UPLOAD);
    return strdup(file_path);
}

// testing unit
int main(){
    char command[] = "$uploads$/path/to/file.txt";

    if (is_command(command)){
        char * file_path = extract_file_path(command);

        if (file_path){
            printf("Command recogninzed for upload");
            free(file_path);
        } else {
            printf("Command not recogninzed for upload");
        }
    }
    return 0;
}