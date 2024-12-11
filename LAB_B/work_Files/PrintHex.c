#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 16 // Number of bytes to read at a time

void PrintHex(unsigned char *buffer, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        printf("%02X ", buffer[i]); // Print each byte as a two-character hexadecimal
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    // Ensure a file name is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(1);
    }

    // Open the file in binary mode
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    unsigned char buffer[CHUNK_SIZE]; // Buffer to hold file data
    size_t bytesRead;

    // Read file in chunks
    while ((bytesRead = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        PrintHex(buffer, bytesRead); // Print the read bytes in hexadecimal
    }

    // Close the file
    fclose(file);
    exit(0);
}
