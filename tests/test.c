#include <stdio.h>
#include <string.h>  

int debug_mode = 1;
int is_add = 1;
FILE* infile = NULL;
FILE* outfile = NULL;
char* encoding_key = "0";
int key_position = 0;
int error_flag = 0;

// Check that c is either a digit or lowercase letter or uppercase letter
int can_encode(char c){
    return ((c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z'));
}
/*
 *   The funciton will *shift* the current *char*. If it's a digit will wrap around 0 to 9, and if it's a letter a to z. 
 *   meaning: 9 + 1 = 0, A - 1 = Z...
 *   key_position = keeps incrementing (happends only after should_encode) until we are out of encoding keys and then wraps around to position 0 and starts over
*/
char wrap_around(char c, int shift){
    if (c >= '0' && c <= '9') {
        c = '0' + (c - '0' + shift + 10)%10;
    }
    else if(c >= 'A' && c <= 'Z'){ 
        c = 'A' + (c - 'A' + shift + 26)%26;
    }
    else{
        c = 'a' + (c - 'a' + shift + 26)%26;
    }
    return c;
}

/*
 *  shift = current shift to the current char [might be negative!]
*/
char encode (char c){
    int shift = 0;
    shift = encoding_key[key_position] - '0'; // from char to integer
    key_position = encoding_key[key_position + 1] ? key_position + 1 : 0;
    shift = (is_add) ? shift : -shift;
    c = (can_encode(c)) ? wrap_around(c,shift) : c;
    return c;
}

void parse_args(int argc, char **argv){
    infile = stdin;
    outfile = stdout;

    for(int i = 1; i < argc; i++){
        //strngcmp from reading material A
        //Part 1
        if(strncmp(argv[i],"-D",2) == 0){
            debug_mode = 0;
        }
        else if(strncmp(argv[i],"+D",2) == 0){
            debug_mode = 1;
        }
        //Part 2 assuming only +E or -E will show but never both.
        else if(argv[i][0] == '+' && argv[i][1] =='E'){
            is_add = 1;
            encoding_key = &argv[i][2];
            key_position = 0;
        }
        else if(argv[i][0] == '-' && argv[i][1] =='E'){
            is_add = 0;
            encoding_key = &argv[i][2];
            key_position = 0;
        }
        else if(argv[i][0] == '-' && argv[i][1] == 'i'){
            infile = fopen(&argv[i][2],"r");
            if(infile == NULL){
                fprintf(stderr,"%s\n","Cannot open file");
                error_flag = 1;; // " Just make sure that if fopen( ) fails, print an error message to stderr and exit. "
                return;
            }
        }
        else if(argv[i][0] == '-' && argv[i][1] == 'o'){
            outfile = fopen(&argv[i][2],"w");
            if(outfile == NULL){
                fprintf(stderr,"%s\n","Cannot open file");
                error_flag = 1;
                return;
            }
        }
        else if(debug_mode){                        
            // DEBUG MODES: if you want to see the above in debug -> remove the else in the beginning of this line. :)
            fprintf(stderr,"%s\n", argv[i]);
        }
    }
}

// Process input character by character
void process_inputs(){
    int c;
    while((c = fgetc(infile)) != EOF){ //feof(infile)
        fputc(encode(c),outfile);
    }
    printf("\n");
}

// Close output streams "normally"
void close_output_stream(){
    if(outfile != stdout){
        fclose(outfile);
    }
}


int main(int argc, char **argv) {

    parse_args(argc,argv);
    if(error_flag){return 1;}
    process_inputs();
    close_output_stream();
    
    return 0;
}