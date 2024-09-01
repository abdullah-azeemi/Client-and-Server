// commandParser.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define COMMAND_UPLOAD "$UPLOAD$"
#define COMMAND_DOWNLOAD "$DOWNLOAD$"

bool is_command(const char *input) {
    return (strncmp(input, COMMAND_UPLOAD, strlen(COMMAND_UPLOAD)) == 0);
}

char *extract_file_path(const char *input) {
    if (!is_command(input)) {
        return NULL;
    }

    const char *file_path = input + strlen(COMMAND_UPLOAD);
    return strdup(file_path);
}

// Testing unit
int main() {
    char command[] = "$UPLOAD$/path/to/file.txt";

    if (is_command(command)) {
        char *file_path = extract_file_path(command);

        if (file_path) {
            printf("Command recognized for upload: %s\n", file_path);
            free(file_path);
        } else {
            printf("Command not recognized for upload.\n");
        }
    }
    return 0;
}
