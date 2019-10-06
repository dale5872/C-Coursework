/*
 * Author: Group 1
 * Adam Alexander Campbell
 * Dale Baker-Allan
 * Fraser Coupland
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void parentProcess()
{
    pid_t mainPID = getpid();
    printf("PID: %d | Parent process start.\n", mainPID);
    // Wait until child process has finished executing.
    wait(NULL);
    printf("PID: %d | Parent process done.\n", mainPID);
}

void childProcess()
{
    pid_t childPID = getpid();
    printf("PID: %d | Child process start.\n", childPID);
    printf("PID: %d | Child process done.\n", childPID);
}

int main()
{
    // Create a fork.
    pid_t childPID = fork();

    if (childPID != 0)
    {
        //run the code for the parent process.
         parentProcess();
    }
    if (childPID == 0)
    {
        //run the code in the child process.
        childProcess();
    }
    return 0;
}
