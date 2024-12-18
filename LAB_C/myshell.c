#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include "LineParser.h"
#include <fcntl.h>
#include <errno.h>

//process manager
    #define TERMINATED  -1
    #define RUNNING 1
    #define SUSPENDED 0

     typedef struct process{
        cmdLine* cmd;                         /* the parsed command line*/
        pid_t pid; 		                  /* the process id that is running the command*/
        int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
        struct process *next;	                  /* next process in chain */
    } process;

    void addProcess(process** process_list, cmdLine* cmd, pid_t pid);
    void removeProcess(pid_t pid);  // Declare removeProcess


int debug = 0;
process* process_list;


/** 
 * functions to hundle backround processes
 * to use need to finish modifiying execute, checkCommand, main acoordenlly 
 * #TODO:  if there is time
**/
#define MAX_JOBS 100

//shell functions
void displayPrompt(){
    char cwd[PATH_MAX];
    if(getcwd(cwd,sizeof(cwd))!= NULL){
        printf ("%s>",cwd);
    }
    else {
        perror ("getcwd error");
    }
}

void readUserInput(char input[], int size){
    fgets(input,size,stdin);
}

cmdLine * parseInput(char input[]){
    return parseCmdLines(input);
}

//handler
void sigchld_handler(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        removeProcess(pid);  // Remove process from the job list
        process* current = process_list; // Update process list status to terminated
        while (current != NULL) {
            if (current->pid == pid) {
                if (WIFEXITED(status)) {
                    current->status = TERMINATED;  // Mark the process as terminated
                } else if (WIFSIGNALED(status)) {
                    current->status = TERMINATED;  // Mark as terminated if killed by a signal
                }
                break;
            }
            current = current->next;
        }
    }
}


void handleInputRedirect(cmdLine *pCmdLine){
    FILE *input_file = fopen(pCmdLine->inputRedirect, "r");
    if(!input_file){
        perror("input redirection error");
        _exit(1);
    }
    dup2(fileno(input_file), STDIN_FILENO);
    fclose(input_file);
}

void handleOutputRedirect(cmdLine *pCmdLine){
    FILE *output_file = fopen(pCmdLine->outputRedirect, "w");
    if(!output_file){
        perror("output redirection error");
        _exit(1);
    }
    dup2(fileno(output_file), STDOUT_FILENO);
    fclose(output_file);
}

//execute
void executePipe(cmdLine *pCmdLine) {
    cmdLine *leftCmd = pCmdLine;
    cmdLine *rightCmd = pCmdLine->next;

    // Validate I/O redirections for pipe
    if (leftCmd->outputRedirect || rightCmd->inputRedirect) {
        fprintf(stderr, "Error: Cannot redirect output of left process or input of right process in a pipeline\n");
        return;
    }

    int pipefd[2];
    pid_t pid1, pid2;

    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe error");
        return;
    }

    // Fork first child (left side of pipe)
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork error - left process");
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }

    if (pid1 == 0) {  // Left child process
        if (debug) {
            fprintf(stderr, "Left Child PID: %d\n", getpid());
            fprintf(stderr, "Executing left command: %s\n", leftCmd->arguments[0]);
        }

        // Close read end of pipe
        close(pipefd[0]);

        // Redirect stdout to write end of pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2 error for left process");
            _exit(1);
        }

        // Close write end of pipe
        close(pipefd[1]);

        // Handle input redirection if present
        if (leftCmd->inputRedirect) {
            handleInputRedirect(leftCmd);
        }

        // Execute left command
        if (execvp(leftCmd->arguments[0], leftCmd->arguments) == -1) {
            perror("execvp error for left command");
            _exit(1);
        }
    }

    // Fork second child (right side of pipe)
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork error - right process");
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }

    if (pid2 == 0) {  // Right child process
        if (debug) {
            fprintf(stderr, "Right Child PID: %d\n", getpid());
            fprintf(stderr, "Executing right command: %s\n", rightCmd->arguments[0]);
        }

        // Close write end of pipe
        close(pipefd[1]);

        // Redirect stdin to read end of pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2 error for right process");
            _exit(1);
        }

        // Close read end of pipe
        close(pipefd[0]);

        // Handle output redirection if present
        if (rightCmd->outputRedirect) {
            handleOutputRedirect(rightCmd);
        }

        // Execute right command
        if (execvp(rightCmd->arguments[0], rightCmd->arguments) == -1) {
            perror("execvp error for right command");
            _exit(1);
        }
    }

    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for child processes if blocking is requested
    if (leftCmd->blocking && rightCmd->blocking) {
        if (debug) {
            fprintf(stderr, "Waiting for both child processes\n");
        }
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    } else {
        if (debug) {
            fprintf(stderr, "Running in background mode\n");
        }
    }
}

void execute(cmdLine *pCmdLine){
    // Check if this is a pipe command
    if (pCmdLine->next) {
        executePipe(pCmdLine);
        return;
    }

    pid_t pid = fork(); 
    if(pid == -1){ 
        perror("fork error - failed to create new process");
        exit(1);
    }

    if(pid == 0){
        if(debug){
            fprintf(stderr, "PID: %d\n", getpid());
            fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
        }

        if(pCmdLine->inputRedirect){
            handleInputRedirect(pCmdLine);
        }

        if(pCmdLine->outputRedirect){
            handleOutputRedirect(pCmdLine);
        }

        if(execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1){
            perror("execvp error");
            _exit(1);
        }
        
    } else {
        addProcess(&process_list, pCmdLine, pid);  // Add to the process list
        

        if (pCmdLine->blocking) { 
            if (debug) {
                fprintf(stderr, "PID: %d\n", pid);
                fprintf(stderr, "Waiting for child process to finish\n");
            }
            waitpid(pid, NULL, 0);
        } else {
            if (debug) {
                fprintf(stderr, "PID: %d\n", pid);
                fprintf(stderr, "Running in background\n");
            }
        }
    }
}

void isDebugMode(int argc, char **argv){
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i],"-d") == 0){
            debug = 1;
            break;
        }
    }
}

void checkQuit(cmdLine* command){
        freeCmdLines(command);
        exit(0);
}

void checkCD (cmdLine* command){
        if(command->argCount < 2 && debug){
            fprintf(stderr, "cd: missing argument\n");
            
        } else {
            if(chdir(command->arguments[1]) != 0 && debug){
                fprintf(stderr,"cd error\n");
            }
        }
}

//procecess checks
void stop_process(int pid) {
    if(kill(pid, SIGSTOP) == -1)
        perror("stop error");
}

void wake_process(int pid) {
    if(kill(pid, SIGCONT) == -1)
        perror("wake error");
}

void term_process(int pid) {
    if(kill(pid, SIGINT) == -1)
        perror("term error");
}

void checkStop(cmdLine* command){
    if(command->argCount > 1){
        stop_process(atoi(command->arguments[1]));
    }
}

void checkWake(cmdLine* command){
    if(command->argCount > 1){
        wake_process(atoi(command->arguments[1]));
    }
}

void checkTerm(cmdLine* command){
    if(command->argCount > 1){
        term_process(atoi(command->arguments[1]));
    }
}

int checkCommand(cmdLine* command){
    if(strcmp(command->arguments[0],"quit") == 0){checkQuit(command);return 1;}
    if(strcmp(command->arguments[0], "cd") == 0){checkCD(command); return 1;}
    if(strcmp(command->arguments[0], "stop") == 0){checkStop(command); return 1;}
    if(strcmp(command->arguments[0], "wake") == 0){checkWake(command); return 1;}
    if(strcmp(command->arguments[0], "term") == 0){checkTerm(command); return 1;}
    return 0;
}


void addProcess(process** process_list, cmdLine* cmd, pid_t pid) {
    process* new_process = (process*) malloc(sizeof(process));
    // Initialize the new process fields
    new_process->cmd = cmd;  // Parsed command line
    new_process->pid = pid;  // Process ID
    new_process->status = RUNNING;  // Initially, the process is running
    new_process->next = *process_list;  // New process points to the current first node
    // Update the process list to point to the new process
    *process_list = new_process;
}

void printProcessList(process** process_list) {
    // Print header
    printf("\nPID          Command      STATUS\n");

    // Iterate through the process list
    process* current = *process_list;
    int index = 0;
    while (current != NULL) {
        // Print process details
        printf("%-12d %-12s %-12s\n", current->pid, current->cmd->arguments[0],
               current->status == RUNNING ? "Running" :
               (current->status == SUSPENDED ? "Suspended" : "Terminated"));
        current = current->next;
        index++;
    }
}

void removeProcess(pid_t pid) {
    process* current = process_list;
    process* previous = NULL;

    while (current != NULL) {
        if (current->pid == pid) {
            if (previous == NULL) {
                process_list = current->next;
            } else {
                previous->next = current->next;
            }
            free(current); 
            return;
        }
        previous = current;
        current = current->next;
    }
    fprintf(stderr, "Error: No process found with pid %d\n", pid);
}


//main helper functions: 
void initializeProcessList(process** process_list) {
    *process_list = NULL;  // Initialize an empty process list
}

void handleProcsCommand(process* process_list) {
    printProcessList(&process_list);  // Print the list of processes
}

int handleUserInput(char input[], cmdLine** command, int argc, char** argv, process** process_list) {
    readUserInput(input, 2048);  
    isDebugMode(argc, argv);     
    if(input[0] == '\n') return 0;  // Skip empty input

    *command = parseInput(input);
    if (*command == NULL) return 0;  // Skip null commands

    if (strcmp((*command)->arguments[0], "procs") == 0) {
        handleProcsCommand(*process_list);
        freeCmdLines(*command);
        return 1;  
    }

    return 0;  
}

void executeCommand(cmdLine* command, process** process_list) {
    if (checkCommand(command)) {
        freeCmdLines(command);
    } else {
        execute(command);  
        freeCmdLines(command);  
    }
}


int main(int argc, char **argv) {
    signal(SIGINT, SIG_IGN);
    signal(SIGCHLD, sigchld_handler);  // Set up SIGCHLD handler to reap child processes
   
    char input[2048];
    cmdLine* command;
    process_list = NULL;  // Declare the process list
    initializeProcessList(&process_list);  

    while (1) {
        displayPrompt();
        if (handleUserInput(input, &command, argc, argv, &process_list)) {
            continue;  // Skip command execution if "procs" command was processed
        }
        executeCommand(command, &process_list);
        sleep(1);  // Sleep to avoid CPU overload
    }
    return 0;
}

