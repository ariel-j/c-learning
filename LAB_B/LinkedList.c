#include <stdio.h>
#include <stdlib.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;


typedef struct link {
    struct link *nextVirus;
    virus *vir;
} link;

void list_print(link *virus_list, FILE *output) {
    while (virus_list != NULL) {
        printVirus(virus_list->vir, output);  // Assuming `printVirus` is implemented
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_signatures(link **virus_list) {
    char filename[256];
    printf("Enter signature file name: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;  // Remove newline

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    virus *v;
    while ((v = readVirus(file)) != NULL) {
        *virus_list = list_append(*virus_list, v);
    }

    fclose(file);
    printf("Signatures loaded.\n");
}

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

int main() {
    link *virus_list = NULL;
    int choice;

    while (1) {
        printf("1) Load signatures\n");
        printf("2) Print signatures\n");
        printf("3) Detect viruses\n");
        printf("4) Fix file\n");
        printf("5) Quit\n");
        printf("Enter choice: ");

        char input[10];
        fgets(input, sizeof(input), stdin);
        sscanf(input, "%d", &choice);

        switch (choice) {
            case 1:
                load_signatures(&virus_list);
                break;
            case 2:
                print_signatures(virus_list);
                break;
            case 3:
                detect_viruses();
                break;
            case 4:
                fix_file();
                break;
            case 5:
                list_free(virus_list);
                return 0;
            default:
                printf("Invalid choice, try again.\n");
        }
    }
}

