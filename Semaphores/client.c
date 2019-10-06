#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "shm.h"
#include "sem.h"

int main()
{
    FILE * fp;
    int shmFd;
    SHMstruct * shmData;
    sem_t * semaphore;

    fp = fopen("/tmp/client.log", "a");
    if(fp == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    //get the semaphore
    semaphore = getSemaphore(SHNAME);
    leaveCriticalRegion(semaphore);

    fprintf(fp, "%s: Client entering critical region\n", getTimeStamp());
    enterCriticalRegion(semaphore);

    shmFd = loadSHM(SHNAME);
    shmData = accessSHM(shmFd); 

    fprintf(fp, "%s: Client leaving critical region\n", getTimeStamp());
    leaveCriticalRegion(semaphore);

    // Remember the condition value!!!
    while(1) 
    {
        //check if the tickets have sold out
        fprintf(fp, "%s: Client entering critical region, checking if sold out\n", getTimeStamp());
        enterCriticalRegion(semaphore);

        if(shmData->soldOut == true) {
            fprintf(fp, "%s: Client leaving critical region\n", getTimeStamp());
            leaveCriticalRegion(semaphore);
            break;
        }
        fprintf(fp, "%s: Client leaving critical region\n", getTimeStamp());
        leaveCriticalRegion(semaphore);

        //take the ticket
        fprintf(fp, "%s: Client entering critical region, taking ticket\n", getTimeStamp());
        enterCriticalRegion(semaphore);

        if(shmData->isTaken == false) {
            shmData->isTaken = true;
            fprintf(stderr, "Ticket %d taken\n", shmData->ticket);
            fprintf(fp, "Ticket %d taken\n", shmData->ticket);

            fprintf(fp, "%s: Client leaving critical region\n", getTimeStamp());
        } else { 
            fprintf(fp, "Ticket %d already taken", shmData->ticket);
        }

        leaveCriticalRegion(semaphore);

        //we have ticket
        sleep((rand() % 4)+1); //random wait time between 1 and 5 seconds

    }

    fprintf(fp, "%s: Client entering critical region, closing SHM\n", getTimeStamp());
    enterCriticalRegion(semaphore);

    clearSHM(shmData);
    closeSHM(shmFd);

    fprintf(fp, "%s: Client leaving critical region\n", getTimeStamp());
    leaveCriticalRegion(semaphore);

    fclose(fp);

    return EXIT_SUCCESS;
}
