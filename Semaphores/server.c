#include <stdio.h>
#include <stdlib.h>

#include "shm.h"
#include "sem.h"

int main()
{
    FILE * fp;
    int shmFd, counter;
    SHMstruct initData = { 1, false, false};
    SHMstruct * shmData;
    sem_t * semaphore;
    
    fp = fopen("/tmp/server.log", "a");
    if(fp == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    //create the semaphore
    semaphore = createSemaphore(SHNAME);
    leaveCriticalRegion(semaphore); //we are in the CR when creating the semaphore

    //enter critical region
    fprintf(fp, "%s: Server entering critical region\n", getTimeStamp());
    enterCriticalRegion(semaphore);

    shmFd = createSHM(SHNAME);
    shmData = initSHM( shmFd, &initData );

    fprintf(fp, "%s: Server leaving critical region\n", getTimeStamp());
    leaveCriticalRegion(semaphore); //leave CR

    fprintf(stderr, "Shared Memory Area created\n");

    counter = 1;
    // Remember the condition value!!!
    while(counter <= MAX_TICKETS)
    {
        fprintf(fp, "%s: Server entering critical region\n", getTimeStamp());
        enterCriticalRegion(semaphore);

        fprintf(stderr, "Ticket %d created\n", shmData->ticket);

        fprintf(fp, "%s: Server leaving critical region\n", getTimeStamp());
        leaveCriticalRegion(semaphore);


        counter++; //increment

        //wait for the ticket to be taken (i.e until critical region unlocks)
        while(1) {
            fprintf(fp, "%s: Server entering critical region, checking ticket\n", getTimeStamp());
            enterCriticalRegion(semaphore);

            if(shmData->isTaken == true) {
                fprintf(fp, "%s: Server leaving critical region\n", getTimeStamp());
                leaveCriticalRegion(semaphore);

                break;
            }
            fprintf(fp, "%s: Server leaving critical region\n", getTimeStamp());
            leaveCriticalRegion(semaphore); //allow someone else to enter CR and take ticket
            sleep(MAX_SLEEP);
        }

        fprintf(fp, "%s: Server entering critical region, new ticket\n", getTimeStamp());
        enterCriticalRegion(semaphore);

        //ticket is taken
        fprintf(stderr, "Ticket %d has been taken\n", shmData->ticket);
        fprintf(fp, "Ticket %d has been taken\n", shmData->ticket);

        //ticket is taken, generate a new ticket
        shmData->ticket = counter;
        shmData->soldOut = false;
        shmData->isTaken = false;

        fprintf(fp, "%s: Server leaving critical region\n", getTimeStamp());
        leaveCriticalRegion(semaphore);
    }

        fprintf(fp, "%s: Server entering critical region, tickets sold out\n", getTimeStamp());
        enterCriticalRegion(semaphore);

        //tickets are sold out
        shmData->ticket = counter;
        shmData->soldOut = true;
        shmData->isTaken = false;

        leaveCriticalRegion(semaphore);

    fprintf(stderr, "Shared Memory Area destroyed\n");

    fprintf(fp, "%s: Server entering critical region and closing shared memory\n", getTimeStamp());
    enterCriticalRegion(semaphore);

    clearSHM(shmData);
    closeSHM(shmFd);
    destroySHM(SHNAME);
    leaveCriticalRegion(semaphore);

    fclose(fp);

    return EXIT_SUCCESS;
}
