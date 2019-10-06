#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "shm.h"

int main()
{
    FILE * fp;
    int shmFd;
    SHMstruct * shmData;

    fp = fopen("/tmp/client.log", "a");
    if(fp == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    shmFd = loadSHM(SHNAME);
    shmData = accessSHM(shmFd); 

    // Remember the condition value!!!
    while(shmData->soldOut == false) 
    {
        //take the ticket
        shmData->isTaken = true;
        fprintf(stderr, "Ticket %d taken\n", shmData->ticket);

        //we have ticket
        sleep((rand() % 4)+1); //random wait time between 1 and 5 seconds

    }

    clearSHM(shmData);
    closeSHM(shmFd);
    fclose(fp);

    return EXIT_SUCCESS;
}
