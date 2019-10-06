/*
 * Author: Group 1
 * Adam Alexander Campbell
 * Dale Baker-Allan
 * Fraser Coupland
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//pipe
int fd1[2];
int fd2[2];

void parentProcess()
{
    char buffer[1];
    char string[] = "1";

    //close parent read
    close(fd1[0]);
    close(fd2[1]);

    pid_t mainPID = getpid();
    printf("PID: %d | Parent process start.\n", mainPID);

    // tell child to go
    write(fd1[1], string, strlen(string));

    //wait for response
    read(fd2[0], buffer, sizeof(buffer)); //waits for input

    printf("PID: %d | Parent process done.\n", mainPID);

    //tell child to go again
    write(fd1[1], string, strlen(string));

}

void childProcess()
{
    char buffer[1];
    char string[] = "2";

    //close child read
    if (close(fd2[0])) perror("closefd2 child");
    if (close(fd2[0])) perror("close fd1 child");

    read(fd1[0], buffer, sizeof(buffer)); //waits for input

    pid_t childPID = getpid();
    printf("PID: %d | Child process start.\n", childPID);

    //write 1 to pipe to signal that the process has started
    write(fd2[1], string, strlen(string));
    close(fd2[1]);
    
    //wait for response to finish
    read(fd1[0], buffer, sizeof(buffer)); //waits for input

    printf("PID: %d | Child process done.\n", childPID);
}

int main()
{
        if(pipe(fd1) == -1) {
            //failed read
            perror("Error opening pipe\n");
            exit(1);
        }
        if(pipe(fd2) == -1) {
            //failed wrtie
            printf("Error opening pipe\n");
            exit(1);
        }

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
