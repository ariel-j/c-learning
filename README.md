# C Programming Learning Repository

This repository contains my work and progress in learning C programming, primarily through hands-on labs and assignments. It covers foundational concepts of C programming, including working with Unix-based systems, debugging, file handling, character encoding, and more. Below, you will find an overview of the skills acquired in Lab 1 and Lab A, as well as what you can expect from the code and tests present in this repository.

## Skills Acquired

### Lab 1: Introduction to C Programming and Debugging
- **Command-Line Arguments**: Gained an understanding of how to handle command-line arguments in C, parsing them with `argc` and `argv`.
- **File I/O**: Learned how to handle file input and output, using `fopen()`, `fgetc()`, `fputc()`, and `fclose()`.
- **Character Encoding**: Implemented a simple character encoder, learning how to manipulate and encode text based on a key, and how to wrap around ASCII values for uppercase, lowercase letters, and digits.
- **Debugging Mode**: Developed a basic debugging mechanism that prints the program’s internal state to `stderr` when enabled, helping with troubleshooting.
- **Standard Input and Output**: Gained experience with the standard input (`stdin`), output (`stdout`), and error (`stderr`) streams in Unix/Linux.
- **Makefile**: Learned to use `make` to automate compilation and cleaning up of project files, streamlining the development process.

### Lab A: Implementing an Encoder
- **Parsing and Handling Command-Line Flags**: Extended knowledge of parsing command-line arguments by handling flags like `-i`, `-o`, `-D`, and `+E` for input, output, debug mode, and encoding operations.
- **Custom Encoding Mechanism**: Implemented a character encoding scheme that adds or subtracts values to characters based on a cyclic key.
- **File Handling**: Used file pointers to read from and write to specified files, supporting dynamic input/output files via command-line arguments.
- **Efficient Debugging**: Integrated robust debugging capabilities into the program, allowing easy tracking of argument processing and internal states.
- **Code Efficiency and Quality**: Focused on code readability and maintaining best practices, ensuring the program can handle different edge cases gracefully.

## Code Overview

### Encoder Program
The main program (`encoder.c`) implements a simple character encoder, which can encode text based on a given encoding key. The program reads from `stdin` by default, but it can also handle custom input files using the `-i` flag and output files with the `-o` flag.

- **Encoding Key**: The program accepts an encoding key via the `+E{key}` or `-E{key}` flag. The key consists of digits that will be added or subtracted to/from the ASCII values of characters in the input text.
- **Debugging Mode**: The `-D` and `+D` flags toggle the debug mode, which prints each command-line argument and helps debug internal states.
- **File Input/Output**: Supports reading from and writing to files, specified by `-i` for input file and `-o` for output file.
- **Character Handling**: Only encodes alphanumeric characters (digits, uppercase, and lowercase letters). Non-alphanumeric characters are passed through unchanged.
- **Makefile**: A `makefile` is provided to build the project with `make encoder` and clean up with `make clean`.

### Testing
The repository includes comprehensive test cases for the encoder program, covering various scenarios like:
- Default encoding behavior (no encoding key).
- Encoding with both addition and subtraction keys.
- Handling of uppercase, lowercase, and digit characters.
- Debug mode output to track program flow.

## Expected Behavior
When you run the program with appropriate command-line arguments, it will:
1. Parse the command-line arguments to configure the encoding behavior, debugging mode, and input/output files.
2. Read characters from the specified input (or `stdin` by default).
3. Encode each character based on the provided key, outputting the result to `stdout` or a specified output file.
4. If in debug mode, it will print the parsed arguments and internal states to `stderr`.

### Example Commands:
1. **Simple Encoding** (adding values to characters):
   ```bash
   ./encoder +E12345
