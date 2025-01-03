#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>       // For getcwd(), fork(), execvp()
#include <linux/limits.h> // For PATH_MAX
#include "LineParser.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define INPUT_BUFFER_SIZE 2048
int debug = 0;

void display_prompt()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s> ", cwd); // Show current working directory
    }
    else
    {
        perror("getcwd failed");
        exit(EXIT_FAILURE);
    }
}

char *read_input()
{
    static char input[INPUT_BUFFER_SIZE];
    if (fgets(input, INPUT_BUFFER_SIZE, stdin) == NULL)
    {
        if (feof(stdin))
        {
            // Handle EOF (Ctrl+D)
            printf("\nEnd of input (EOF) detected. Exiting...\n");
            exit(0);
        }
        else
        {
            perror("Error reading input");
            return NULL;
        }
    }
    input[strcspn(input, "\n")] = '\0';
    return input;
}

void handle_redirection(cmdLine *pCmdLine)
{
    if (pCmdLine->inputRedirect)
    {
        int inputFd = open(pCmdLine->inputRedirect, O_RDONLY);
        if (inputFd == -1)
        {
            perror("Failed to open input file");
            exit(1);
        }
        dup2(inputFd, STDIN_FILENO);
        close(inputFd);
    }

    if (pCmdLine->outputRedirect)
    {
        int outputFd = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outputFd == -1)
        {
            perror("Failed to open output file");
            exit(1);
        }
        dup2(outputFd, STDOUT_FILENO);
        close(outputFd);
    }
}

void run_child_process(cmdLine *pCmdLine)
{
    handle_redirection(pCmdLine);
    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
    perror("execvp failed");
    _exit(1);
    }
}

// Handles parent process behavior (foreground/background execution)
void run_parent_process(pid_t pid, int blocking)
{
    if (blocking) {
        waitpid(pid, NULL, 0);
    }
    else {
        // Background: Notify user of background process
        printf("Started background process with PID: %d\n", pid);
    }
}

// free memory and exit gracefully
void cleanup_and_exit(cmdLine *parsedLine)
{
    freeCmdLines(parsedLine);
}

void handle_cd(cmdLine *parsedLine)
{
    if (parsedLine->argCount < 2)
    {
        fprintf(stderr, "cd: missing argument\n");
    }
    else
    {
        if (chdir(parsedLine->arguments[1]) != 0)
        {
            perror("cd");
        }
    }
}

void send_signal(int pid, int signal)
{
    if (kill(pid, signal) == -1)
    {
        perror("Failed to send signal");
    }
    else
    {
        printf("Signal %d sent to process %d\n", signal, pid);
    }
}

void handle_stop_command(cmdLine *pCmdLine)
{
    if (pCmdLine->argCount != 2) {
        fprintf(stderr, "stop\n");
        return;
    }
    int pid = atoi(pCmdLine->arguments[1]);
    send_signal(pid, SIGSTOP);
}

void handle_wake_command(cmdLine *pCmdLine)
{
    if (pCmdLine->argCount != 2) {
        fprintf(stderr, "Usage: wake\n");
        return;
    }
    int pid = atoi(pCmdLine->arguments[1]);
    send_signal(pid, SIGCONT);
}

void handle_term_command(cmdLine *pCmdLine) {
    if (pCmdLine->argCount != 2)
    {
        fprintf(stderr, "Usage: term <process id>\n");
        return;
    }
    int pid = atoi(pCmdLine->arguments[1]);
    send_signal(pid, SIGINT);
}


void isDebugMode(int argc, char **argv){
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i],"-d") == 0){
            debug = 1;
            break;
        }
    }
}
void execute(cmdLine *pCmdLine) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }
    else if (pid == 0) {
        run_child_process(pCmdLine);
    }
    else {
        run_parent_process(pid, pCmdLine->blocking);
    }
}

int main()
{
    char *input;
    cmdLine *parsedLine;

    while (1) {
        display_prompt();
        input = read_input();
        if (!input)
            continue; // Retry loop if input reading fails
        parsedLine = parseCmdLines(input);
        if (!parsedLine)
            continue;

        // Handle "quit" command
        if (strcmp(parsedLine->arguments[0], "quit") == 0) {
            cleanup_and_exit(parsedLine);
            break;
        }

        // Handle "cd" command
        if (strcmp(parsedLine->arguments[0], "cd") == 0) {
            handle_cd(parsedLine);
            cleanup_and_exit(parsedLine);
            continue; // Skip execution for "cd"
        }

        if (strcmp(parsedLine->arguments[0], "stop") == 0)
        {
            handle_stop_command(parsedLine);
            cleanup_and_exit(parsedLine);
            continue;
        }
        if (strcmp(parsedLine->arguments[0], "wake") == 0)
        {
            handle_wake_command(parsedLine);
            cleanup_and_exit(parsedLine);
            continue;
        }
        if (strcmp(parsedLine->arguments[0], "term") == 0) {
            handle_term_command(parsedLine);
            cleanup_and_exit(parsedLine);
            continue;
        }
        execute(parsedLine);
        cleanup_and_exit(parsedLine);
    }

    return 0;
}
