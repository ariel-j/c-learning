#include <stdio.h>
#include <string.h>  


int debug_mode = 1;
FILE *infile = NULL;
FILE *outfile = NULL;
char *encoding_key = "0";
int addition = 1;

int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

int is_upper_case(char c) {
    return (c >= 'A' && c <= 'Z');
}

int is_lower_case(char c) {
    return (c >= 'a' && c <= 'z');
}

char encode(char c) {
    static int key_index = 0;
    char digit = encoding_key[key_index];
    int value = digit - '0';

    if (is_digit(c)) {
        c = addition ? (c - '0' + value) % 10 + '0' : (c - '0' - value + 10) % 10 + '0';
    } else if (is_upper_case(c)) {
        c = addition ? (c - 'A' + value) % 26 + 'A' : (c - 'A' - value + 26) % 26 + 'A';
    } else if (is_lower_case(c)) {
        c = addition ? (c - 'a' + value) % 26 + 'a' : (c - 'a' - value + 26) % 26 + 'a';
    }

    key_index = (key_index + 1) % (encoding_key[0] == '0' ? 1 : strlen(encoding_key));
    return c;
}

void print_debug_info(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (debug_mode) {
            fprintf(stderr, "Argument %d: %s\n", i, argv[i]);
        }

        if (argv[i][0] == '-' && argv[i][1] == 'D') {
            debug_mode = 0;
        } else if (argv[i][0] == '+' && argv[i][1] == 'D') {
            debug_mode = 1;
        } else if (argv[i][0] == '+' && argv[i][1] == 'E') {
            encoding_key = argv[i] + 2;
            addition = 1;
        } else if (argv[i][0] == '-' && argv[i][1] == 'E') {
            encoding_key = argv[i] + 2;
            addition = 0;
        } else if (argv[i][0] == '-' && argv[i][1] == 'i') {
            infile = fopen(argv[i] + 2, "r");
            if (infile == NULL) {
                fprintf(stderr, "Error: Could not open input file %s\n", argv[i] + 2);
                exit(1);
            }
        } else if (argv[i][0] == '-' && argv[i][1] == 'o') {
            outfile = fopen(argv[i] + 2, "w");
            if (outfile == NULL) {
                fprintf(stderr, "Error: Could not open output file %s\n", argv[i] + 2);
                exit(1);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    infile = stdin;
    outfile = stdout;

    print_debug_info(argc, argv);

    int c;
    while ((c = fgetc(infile)) != EOF) {
        c = encode(c);
        fputc(c, outfile);
    }

    if (infile != stdin) fclose(infile);
    if (outfile != stdout) fclose(outfile);

    return 0;
}
