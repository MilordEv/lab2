#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


int programmCount(char* command);
int wordsCount(char* args);
char*** getArgs(char* command);

int main() {
    
    printf(">: ");
    char command[8191];
    fgets(command, sizeof(command), stdin);

    kill(getppid(), SIGUSR1);

    int argsCount = programmCount(command);
    
    char*** args = getArgs(command);

    int pipe1[2] = {-1, -1};
    int pipe2[2];

    for (int i = 0; i < argsCount; i++) {
        if (i != argsCount - 1) {
            pipe(pipe2);
        }

        pid_t child_pid = fork();

        if (child_pid < 0) {
            printf("An error has occurred. Couldn't run the K program\n");
            for (int i = 0; i < argsCount; i++) {
                free(args[i]);
            }
            if (args) {
                free(args);
            }
            exit(EXIT_FAILURE);
        } else if (child_pid == 0) {
            if (pipe1[0] != -1) {
                close(pipe1[1]);
                dup2(pipe1[0], 0);
            }

            if (i != argsCount - 1) {
                close(pipe2[0]);
                dup2(pipe2[1], 1);
            }

            execvp(args[i][0], args[i]);

        } else {
            close(pipe1[0]);
            close(pipe1[1]);
            pipe1[0] = pipe2[0];
            pipe1[1] = pipe2[1];

            int wstatus;
            wait(&wstatus);

            if (wstatus != 0) {
                for (int i = 0; i < argsCount; i++) {
                    free(args[i]);
                }
                if (args) {
                    free(args);
                }
                exit(EXIT_FAILURE);
            }
        }
    }

    for (int i = 0; i < argsCount; i++) {
        free(args[i]);
    }
    free(args);

    return 0;
}


int programmCount(char* command) {
    if (!command) {
        return -1;
    }

    int count = 1;

    int i = 0;
    while ((command[i] != '\0') && (command[i] != '\n')) {

        if (command[i] == '"') {
            while(command[++i] != '"') {}
        }

        if (command[i] == '|') {
            count++;        
        }

        i++;
    }

    return count;
}

int wordsCount(char* args) {
    if (!args) {
        return -1;
    }

    int count = 0;

    int i = 0;
    while ((args[i] != '\0') && (args[i] != '\n')) {

        if (args[i] == '"') {
            while (args[++i] != '"') {}
            count++;
            while (args[++i] == ' ') {}
            continue;
        }

        while ((args[i] != ' ') && (args[i] != '\0') && (args[i++] != '\n')) {}
        count++;
        if (args[i] == ' ') {
            while (args[++i] == ' ') {}
        }

    }

    return count;
}

char*** getArgs(char* command) {

    int argsCount = programmCount(command);
    if (argsCount < 0) {
        exit(EXIT_FAILURE);
    }

    char*** args = (char***) malloc(argsCount * sizeof(char**));

    int i = 0;
    int progNumber = 0;
    int strStart = 0;
    while ((command[i] != '\0') && (command[i] != '\n')) {
        i++;
        
        if (command[i] == '"') {
            while(command[i++] != '"') {}
        }

        if ((command[i] == '|') || (command[i] == '\0') || (command[i] == '\n')) {
            
            command[i] = '\0';
            char* arg = (char*) malloc((i - strStart + 1) * sizeof(char));

            memcpy(arg, command + strStart * sizeof(char), i - strStart);

            int wordCount = wordsCount(arg);
            if (wordCount < 0) {
                exit(EXIT_FAILURE);
            }

            args[progNumber] = (char**) malloc((wordCount + 1) * sizeof(char*)); 

            char* temp = strtok(arg, " \n\0");
            int j = 0;
            while(temp != NULL) {
                args[progNumber][j] = temp;
                j++;
                temp = strtok(NULL, " \n\0");
            }
            args[progNumber][wordCount] = NULL;

            progNumber++;

            while (command[++i] == ' ') {}
            strStart = i;
        }
    }

    return args;
}