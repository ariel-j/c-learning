#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>
#include <string.h>

// ============================== Constants ==============================

#define MAX_INPUT_SIZE 100
#define MAX_BUFFER_SIZE 10000
#define MAX_FILENAME_SIZE 128
#define MIN_UNIT_SIZE 1
#define MAX_UNIT_SIZE 4
#define BUFFER_SIZE 256
#define MAX_FILES 2


// ============================== Error codes ==============================
typedef enum {
    SUCCESS = 0,
    ERROR_FILE_EMPTY = -1,
    ERROR_FILE_OPEN = -2,
    ERROR_INPUT = -3,
    ERROR_BOUNDS = -4,
    ERROR_LOCATION = -5
} error_code;

//============================== Format strings ==============================

// static const char* hex_formats[] = {"%hhx\n", "%hx\n", "No such unit", "%x\n"};
// static const char* dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};

// ============================== Structs ==============================

typedef struct {
    char debug_mode;
    char file_name[MAX_FILENAME_SIZE];
    int unit_size;
    unsigned char mem_buf[MAX_BUFFER_SIZE];
    size_t mem_count;
    char display_mode;
} state;

typedef struct fun_desc{
    char *name;
    void (*func)();
} fun_desc;

typedef struct {
    int fd;
    void *map_start;
    size_t file_size;
    Elf32_Ehdr *elf_header;
} ElfFile;

// ============================ Global variables ===================================
static char debug_mode = 0;
int file_count = 0;
ElfFile elf_files[MAX_FILES];

// ============================== Utility Functions ==============================

int trim_newline_len(char input[]) {
    int len = strlen(input);
    if (len > 0 && input[len-1] == '\n') {
        input[len-1] = '\0';
        len--;
    }
    return len;
}

error_code validate_input(const char* input, int* result, int min, int max) {
    if (!input || !result) return ERROR_INPUT;
    
    char* endptr;
    long val = strtol(input, &endptr, 10);
    
    if (endptr == input || *endptr != '\n') return ERROR_INPUT;
    if (val < min || val > max) return ERROR_BOUNDS;
    
    *result = (int)val;
    return SUCCESS;
}

int validation(int len, char input []) {
    if (len == 0 || len >= 5 - 1) return 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] < '0' || input[i] > '9') {
            return 0;
        }
    }
    return 1;
}

// ============================== Debug Functions ==============================

void print_debug_info(const state* s) {
    fprintf(stderr, "Debug Information:\n");
    fprintf(stderr, "Unit Size: %d\n", s->unit_size);
    fprintf(stderr, "File Name: %s\n", s->file_name);
    fprintf(stderr, "Memory Count: %zu\n", s->mem_count);
}


// Toggles the debug mode, printing the current state.
void toggle_debug_mode() {
    debug_mode = !debug_mode;
    printf("Debug mode %s\n", debug_mode ? "ON" : "OFF");
}

// Unmaps memory region mapped by mmap and closes the file descriptor to ensure proper resource deallocation.
void unmap_and_close(ElfFile *elf_file) {
    if (elf_file->map_start) {
        munmap(elf_file->map_start, elf_file->file_size);  // Unmaps the memory region.
        elf_file->map_start = NULL;
    }
    if (elf_file->fd >= 0) {
        close(elf_file->fd);  // Closes the file descriptor.
        elf_file->fd = -1;
    }
}

// ============================= Menu Functions ==============================

// ============================= 0 =============================

void examine_elf_file() {

    if (file_count >= MAX_FILES) {
        printf("Cannot open more than %d ELF files\n", MAX_FILES);
        return;
    }

    char file_name[BUFFER_SIZE];
    printf("Enter ELF file name: ");
    fgets(file_name, BUFFER_SIZE, stdin);
    file_name[strcspn(file_name, "\n")] = 0;  // Remove trailing newline.

    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return;
    }

    size_t file_size = lseek(fd, 0, SEEK_END);  // Get the file size.
    lseek(fd, 0, SEEK_SET);  // Reset the file offset.

    void *map_start = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);  // Map the file into memory.
    if (map_start == MAP_FAILED) {
        perror("Failed to mmap file");
        close(fd);
        return;
    }

    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)map_start;  // Cast the mapped memory to an ELF header structure.

    // Verify the ELF magic number to confirm it's a valid ELF file.
    if (memcmp(elf_header->e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Not a valid ELF file\n");
        munmap(map_start, file_size);
        close(fd);
        return;
    }

    // Store the ELF file information.
    elf_files[file_count].fd = fd;
    elf_files[file_count].map_start = map_start;
    elf_files[file_count].file_size = file_size;
    elf_files[file_count].elf_header = elf_header;
    file_count++;

    // Print ELF header details.
    printf("Magic: %c%c%c\n", elf_header->e_ident[EI_MAG1], elf_header->e_ident[EI_MAG2], elf_header->e_ident[EI_MAG3]);
    printf("Data: %s\n", elf_header->e_ident[EI_DATA] == ELFDATA2LSB ? "Little Endian" : "Big Endian");
    printf("Entry point: 0x%x\n", elf_header->e_entry);
    printf("Section header table offset: 0x%x\n", elf_header->e_shoff);
    printf("Number of section headers: %d\n", elf_header->e_shnum);
    printf("Size of section headers: %d\n", elf_header->e_shentsize);
    printf("Program header table offset: 0x%x\n", elf_header->e_phoff);
    printf("Number of program headers: %d\n", elf_header->e_phnum);
    printf("Size of program headers: %d\n", elf_header->e_phentsize);
}


void print_section_names() {
    printf("Not implemented yet\n");
}

void print_symbols() {
    printf("Not implemented yet\n");
}

void check_files_for_merge() {
    printf("Not implemented yet\n");
}

void merge_elf_files() {
    printf("Not implemented yet\n");
}

// Unmaps any resources and exits the program.
void quit() {
    for (int i = 0; i < file_count; i++) {
        unmap_and_close(&elf_files[i]);
    }
    printf("Exiting program.\n");
    exit(0);
}

struct fun_desc menu[] = { 
    {"Toggle Debug Mode", toggle_debug_mode},
        {"Examine ELF File", examine_elf_file},
        {"Print Section Names", print_section_names},
        {"Print Symbols", print_symbols},
        {"Check Files for Merge", check_files_for_merge},
        {"Merge ELF Files", merge_elf_files},
        {"Quit", quit},
        {NULL, NULL}

};

void display_menu(void) {
    printf("\nChoose action:\n");
    for(int i = 0; menu[i].name != NULL; i++) {
        printf("%d-%s\n", i, menu[i].name);
    }
}

// Main function presenting a menu for user actions.
int main() {
    //int bounds = (sizeof(menu) / sizeof(menu[0])) - 1; // -1 for NULL terminator
    state s = {
        .debug_mode = 0,
        .unit_size = 1,
        .display_mode = 0,
        .mem_count = 0,
        .file_name = ""
    };
       
    while (1) {
        if(s.debug_mode) {
            print_debug_info(&s);
        }

        display_menu();
        printf("Option: ");
        int option;
        scanf("%d", &option);
        getchar();  // Consume newline character.

        if (option >= 0 && menu[option].func != NULL) {
            menu[option].func();
        } else {
            printf("Invalid option\n");
        }
    }

    return 0;
}


//gcc -m32 -o myELF_program ELFmenu.c
