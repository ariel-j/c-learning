#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <LAB_2/LineParser.h>

void execute(cmdLine *pCmdLine) {
    // Fork a new process
    pid_t pid = fork();
    if (pid == 0) { // Child process
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
            perror("Execution failed");
            _exit(1);
        }
    } else if (pid < 0) { // Fork failed
        perror("Fork failed");
    } else { // Parent process
        if (pCmdLine->blocking) {
            waitpid(pid, NULL, 0); // Wait for child process
        }
    }
}

int main() {
    char cwd[PATH_MAX]; // Buffer for current working directory
    char input[2048];   // Input buffer
    cmdLine *parsedLine;

    while (1) {
        // Display prompt
        if (getcwd(cwd, PATH_MAX)) {
            printf("%s> ", cwd);
        } else {
            perror("getcwd failed");
            continue;
        }

        // Read input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets failed");
            continue;
        }

        // Remove newline character
        input[strcspn(input, "\n")] = 0;

        // Exit on "quit"
        if (strcmp(input, "quit") == 0) {
            break;
        }

        // Parse input
        parsedLine = parseCmdLines(input);
        if (parsedLine == NULL) {
            continue;
        }

        // Execute command
        execute(parsedLine);

        // Free resources
        freeCmdLines(parsedLine);
    }

    return 0;
}
