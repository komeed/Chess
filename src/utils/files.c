//
// Created by Omeed on 2/3/26.
//

#include "files.h"

char* read_file(const char* path) {
    FILE* file = fopen(path, "rb"); // open in binary mode
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);         // go to end
    long length = ftell(file);        // get file size
    fseek(file, 0, SEEK_SET);         // go back to start

    char* buffer = malloc(length + 1); // +1 for null terminator
    if (!buffer) {
        fprintf(stderr, "Failed to allocate memory for file: %s\n", path);
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, length, file);
    if (bytesRead != length) {
        fprintf(stderr, "Failed to read file: %s\n", path);
        free(buffer);
        fclose(file);
        return NULL;
    }
    buffer[length] = '\0';            // null terminate

    fclose(file);
    return buffer;
}
