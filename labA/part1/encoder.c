#include <stdio.h>
#include <string.h>

int debug_mode = 1; 
FILE *infile = NULL;
FILE *outfile = NULL;

char encode(char c) {
    return c;
}

void print_debug_char(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (debug_mode) {
            fprintf(stderr, "Argument %d: %s\n", i, argv[i]);
        }
        if (strcmp(argv[i], "-D") == 0) {
            debug_mode = 0; 
        } else if (strcmp(argv[i], "+D") == 0) {
            debug_mode = 1; 
        }
    }
}

int main(int argc, char *argv[]) {
    infile = stdin;
    outfile = stdout;
    print_debug_char(argc, argv);
    int c;
    while ((c = fgetc(infile)) != EOF) {
        c = encode(c); 
        fputc(c, outfile);
    }
    fclose(outfile);
    return 0;
}
