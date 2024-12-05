#include <stdio.h>
#include <stdlib.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

virus* allocateVirus() {
    virus* v = malloc(sizeof(virus));
    if (!v) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return v;
}

int readUnsignedShort(unsigned short* value, FILE* file) {
    return fread(value, sizeof(unsigned short), 1, file) == 1;
}

unsigned char* readSignature(unsigned short size, FILE* file) {
    unsigned char* sig = malloc(size);
    if (!sig) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    if (fread(sig, sizeof(unsigned char), size, file) != size) {
        free(sig);
        return NULL;  // Error reading signature
    }
    return sig;
}

int readFixedSizeString(char* buffer, size_t size, FILE* file) {
    return fread(buffer, sizeof(char), size, file) == size;
}


virus* readVirus(FILE* file) {
    virus* v = allocateVirus();
    if (!readUnsignedShort(&v->SigSize, file)) {
        free(v);
        return NULL;  // EOF or read error
    }
    if (!readFixedSizeString(v->virusName, 16, file)) {
        free(v);
        return NULL;  
    }
    v->sig = readSignature(v->SigSize, file);
    if (!v->sig) {
        free(v);
        return NULL;  
    }

    return v;
}

void printVirus(virus* v, FILE* output) {
    fprintf(output, "Virus Name: %s\n", v->virusName);
    fprintf(output, "Signature Length: %u\n", v->SigSize);
    
    fprintf(output, "Signature: ");
    for (unsigned short i = 0; i < v->SigSize; i++) {
        fprintf(output, "%02X ", v->sig[i]);
    }
    fprintf(output, "\n");
}

void check_magic_number (char magic[], FILE* file) {
    printf("%c%c%c%c\n", magic[0], magic[1], magic[2], magic[3]);
    if (magic[0] == 0x56 && magic[1] == 0x49 && magic[2] == 0x52 && magic[3] == 0x4C) {
        printf("Little-endian file detected.\n");
    } else if (magic[0] == 0x56 && magic[1] == 0x49 && magic[2] == 0x52 && magic[3] == 0x42) {
        printf("Big-endian file detected.\n");
    } else {
        fprintf(stderr, "Invalid magic number.\n");
        fclose(file);
        exit(1);
    }

}

void read_and_print_virus_descriptions(FILE* file) {
    virus* v;
    while ((v = readVirus(file)) != NULL) {
        printVirus(v, stdout);  
        free(v->sig);           
        free(v);                
    }
}



int main(int argc, char* argv[]) {
        
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <signatures_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "rb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }
 
    unsigned char magic[4];
    fread(magic, sizeof(unsigned char), 4, file); 
    check_magic_number(magic, file); 
    read_and_print_virus_descriptions(file);
    fclose(file);
    return 0;
}



// %d: for printing integers
// %f: for printing floating-point numbers
// %c: for printing characters
// %s: for printing strings
// %p: for printing memory addresses
// %x: for printing hexadecimal values