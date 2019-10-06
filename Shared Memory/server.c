#include <stdio.h>
#include <stdlib.h>

#include "shm.h"

int main()
{
    FILE * fp;
    int shmFd, counter;
    SHMstruct initData = { 1, false, false};
    SHMstruct * shmData;
    
    fp = fopen("/tmp/server.log", "a");
    if(fp == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    shmFd = createSHM(SHNAME);
    shmData = initSHM( shmFd, &initData );
    fprintf(stderr, "Shared Memory Area created\n");

    counter = 1;
    // Remember the condition value!!!
    while(counter <= MAX_TICKETS)
    {
        fprintf(stderr, "Ticket %d created\n", shmData->ticket);
        counter++; //increment

        //wait for the ticket to be taken
        while(shmData->isTaken == false) {
            wait(NULL);
        }

        //ticket is taken
        fprintf(stderr, "Ticket %d has been taken\n", shmData->ticket);

        //ticket is taken, generate a new ticket
        shmData->ticket = counter;
        shmData->soldOut = false;
        shmData->isTaken = false;
    }

    //tickets are sold out
        shmData->ticket = counter;
        shmData->soldOut = true;
        shmData->isTaken = false;

    fprintf(stderr, "Shared Memory Area destroyed\n");
    clearSHM(shmData);
    closeSHM(shmFd);
    destroySHM(SHNAME);
    fclose(fp);

    return EXIT_SUCCESS;
}
