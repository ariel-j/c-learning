#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int LittleEndian = 0; 
char *fileName = NULL;

//struts
typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

typedef struct link {
    struct link *nextVirus;
    virus *vir;
} link;

struct fun_desc {
    char *name;
    void (*fun)(link **);
};


//declaretins
void print_menu(struct fun_desc *menu);
void load_signatures(link **virus_list);
void list_free(link *virus_list);
link* list_append(link *virus_list, virus *data);
void list_print(link *virus_list, FILE *output);
void fix_file();
void print_signatures(link *virus_list);
void read_and_print_virus_descriptions(FILE* file);
void check_magic_number (char magic[], FILE* file);
virus* readVirus(FILE* file);
int readFixedSizeString(char* buffer, size_t size, FILE* file);
unsigned char* readSignature(unsigned short size, FILE* file);
virus* allocateVirus();


//virusDetector

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

//LinkedList
void print_signatures(link *virus_list) {
    if (virus_list == NULL) {
        printf("No signatures loaded.\n");
        return;
    }
    list_print(virus_list, stdout);
}

void detect_viruses() {
    printf("Not implemented\n");
}

void fix_file() {
    printf("Not implemented\n");
}

void list_print(link *virus_list, FILE *output) {
    while (virus_list != NULL) {
        printVirus(virus_list->vir, output); 
        fprintf(output, "\n");
        virus_list = virus_list->nextVirus;
    }
}

link* list_append(link *virus_list, virus *data) {
    link *newLink = malloc(sizeof(link));
    if (!newLink) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    newLink->vir = data;
    newLink->nextVirus = NULL;

    if (virus_list == NULL) {
        return newLink;  // New list with one element
    }

    // Option 1: Add to the beginning
    newLink->nextVirus = virus_list;
    return newLink;

    // Option 2: Add to the end
    /*
    link *current = virus_list;
    while (current->nextVirus != NULL) {
        current = current->nextVirus;
    }
    current->nextVirus = newLink;
    return virus_list;
    */
}

void list_free(link *virus_list) {
    while (virus_list != NULL) {
        link *temp = virus_list;
        virus_list = virus_list->nextVirus;
        free(temp->vir->sig);  // Free the virus signature
        free(temp->vir);       // Free the virus structure
        free(temp);            // Free the link itself
    }
}

void verifying_magic_number (FILE *file) {
    char magicNumber[5] = {0};
    if (fread(magicNumber, 1, 4, file) != 4) {
        fprintf(stderr, "Error with magic number\n");
        fclose(file);
        return;
    }

    LittleEndian = (strcmp(magicNumber, "VIRL") == 0);
    if (!LittleEndian && strcmp(magicNumber, "VIRB") != 0) {
        printf("Invalid magic number: %s\n", magicNumber);
        fclose(file);
        return;
    }
    printf("Magic number verified: %s\n", magicNumber);

}


void uploading_file(FILE *file, link **virus_list){
    list_free(*virus_list);
    *virus_list = NULL;
    virus *v;
    while ((v = readVirus(file)) != NULL) {
        *virus_list = list_append(*virus_list, v);
    }
    fclose(file);
    printf("Signatures loaded successfully.\n");
}


void load_signatures(link **virus_list) {
    char filename[256];
    printf("Enter signature file name: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        printf("Failed to read file name.\n");
        return;
    }

    filename[strcspn(filename, "\n")] = 0;  
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }
    virus *v;
    verifying_magic_number(file);
    uploading_file(file, virus_list);
    
    }


//menu
struct fun_desc menu[] = {
    {"Load signatures", load_signatures},
    {"Print signatures", (void (*)(link **))print_signatures},
    {"Detect viruses", (void (*)(link **))detect_viruses},
    {"Fix file", (void (*)(link **))fix_file},
    {"Quit", (void (*)(link **))list_free},
    {NULL, NULL},
};

void print_menu(struct fun_desc *menu)
{
    printf("Select a function from 1-5:\n");
    for (int i = 0; menu[i].name != NULL; i++)
    {
        printf("%d) %s\n", i+1, menu[i].name);
    }
}

void handle_choice(int choice, link **virus_list) {
    switch (choice) {
        case 1:
            load_signatures(virus_list);
            break;
        case 2:
            print_signatures(*virus_list);
            break;
        case 3:
            detect_viruses();
            break;
        case 4:
            fix_file();
            break;
        case 5:
            list_free(*virus_list);
            exit(0);
        default:
            printf("Invalid choice, try again.\n");
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
    char input[100];
    link *virus_list = NULL;
    fileName = argv[1];
    int userChoise;
    while (1) {
            print_menu(menu);
            if (fgets(input, sizeof(input), stdin) == NULL) break;
            sscanf(input, "%d", &userChoise);
            if (userChoise < 0 || userChoise >= sizeof(menu)){
                printf("not within bounds\n");
                exit(0);
                }
            printf("within bounds\n");
            printf("%d \n", userChoise);
            handle_choice(userChoise, &virus_list);   
        }

    list_free(virus_list);
    return 0;
    // unsigned char magic[4];
    // fread(magic, sizeof(unsigned char), 4, file); 
    // check_magic_number(magic, file); 
    // read_and_print_virus_descriptions(file);
    // fclose(file);
    // return 0;
}
