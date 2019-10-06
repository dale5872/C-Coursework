#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "shm.h"
#include "sem.h"

int main()
{
    fprintf(stderr, "%s: Client started\n", getTimeStamp());
    FILE * fp;
    int shmFd;
    SHMstruct * shmData;
    sem_t * semaphore;
    int channel;

    fprintf(stderr, "%s: Opening Logfile\n", getTimeStamp());

    fp = fopen("/tmp/client.log", "a");
    if(fp == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "%s: Logfile opened\n", getTimeStamp());

    fprintf(fp, "%s: Starting client...\n", getTimeStamp());

    //get the semaphore
    fprintf(fp, "%s: Opening shared memory block and entering critical region\n", getTimeStamp());
    semaphore = getSemaphore(SHNAME);

    enterCriticalRegion(semaphore);

    shmFd = loadSHM(SHNAME);
    shmData = accessSHM(shmFd); 


    //get channel number
    shmData[0].isTaken = true;
    channel = shmData[0].ticket;
    fprintf(stderr, "Channel number %d\n", channel);
    fprintf(fp, "Channel number %d\n", channel);

    fprintf(fp, "%s: Client leaving critical region\n", getTimeStamp());
    leaveCriticalRegion(semaphore);

    //wait until channels have been assinged
    while(shmData[0].soldOut == false) {
        sleep(1);
    }

    // Remember the condition value!!!
    while(1) 
    {
        //check if the tickets have sold out
        if(shmData[channel].soldOut == true) {
            fprintf(fp, "Tickets sold out\n");
            break;
        }

        if(shmData[channel].isTaken == false) {
            shmData[channel].isTaken = true;
            fprintf(stderr, "Ticket %d taken\n", shmData[channel].ticket);
            fprintf(fp, "Ticket %d taken\n", shmData[channel].ticket);

        } else { 
            fprintf(fp, "Ticket %d already taken", shmData[channel].ticket);
        }

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
