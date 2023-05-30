#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

pid_t pid_child;

void signal_for_B(int sig) {
    int wstatus;
    clock_t clockStart = clock();
    int flag = 1;

    while((float)(clock() - clockStart) / CLOCKS_PER_SEC < 3 ) {
        if (waitpid(pid_child, &wstatus, WNOHANG) < 0) {
            flag = 0;
            break;  
        }
    }
    if (flag && (waitpid(pid_child, &wstatus, WNOHANG) == 0)) {
        kill(pid_child, SIGTERM);
        printf("Program B is killed\n");  
    }
}

int main() {
    
    pid_child = fork();
    if (pid_child < 0) {
        printf("An error occurred when trying to start a child process from process A");
        return 1;
    }

    if (!pid_child) {
        char* name_B = "./progB.o";
        execl(name_B, name_B, NULL);
    } else {
        signal(SIGUSR1, signal_for_B);
    }

    int child_status = 0;
    wait(&child_status);

    return 0;
}