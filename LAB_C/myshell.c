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

int debug = 0;


/** 
 * functions to hundle backround processes
 * to use need to finish modifiying execute, checkCommand, main acoordenlly 
 * #TODO:  if there is time
**/
#define MAX_JOBS 100
typedef struct {
    int pid;
    char command[PATH_MAX];
} job;

job job_list[MAX_JOBS];
int job_count = 0;

void addJob(int pid, const char *command) {
    if (job_count < MAX_JOBS) {
        job_list[job_count].pid = pid;
        strncpy(job_list[job_count].command, command, PATH_MAX);
        job_count++;
    } else {
        fprintf(stderr, "Job list full, cannot add more jobs\n");
    }
}

void removeJob(int pid) {
    for (int i = 0; i < job_count; i++) {
        if (job_list[i].pid == pid) {
            for (int j = i; j < job_count - 1; j++) {
                job_list[j] = job_list[j + 1];
            }
            job_count--;
            break;
        }
    }
}

void listJobs() {
    for (int i = 0; i < job_count; i++) {
        printf("[%d] %d %s\n", i + 1, job_list[i].pid, job_list[i].command);
    }
}

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
        removeJob(pid);
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

//procecess checkx
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

int main(int argc, char **argv)
{
    signal(SIGINT, SIG_IGN);
    int size = 2048;
    char input[size];
    cmdLine* command;
    while (1)
    {
        displayPrompt();
        readUserInput(input,size);
        isDebugMode(argc,argv);
        if(input[0] == '\n') continue; // skip empty input.
        command = parseInput(input);
        if(command == NULL) continue; // skip null commands.
        if(checkCommand(command)) continue;
        execute(command);
        freeCmdLines(command);
        sleep(1);
    }

    return 0;
}