#include <stdio.h>
#include <stdlib.h>

#include "shm.h"
#include "sem.h"

int main()
{
    FILE * fp;
    int shmFd, counter, channelCounter, ticketNumber;
    SHMstruct initData = { 1, false, false};
    SHMstruct * shmData;
    sem_t * semaphore;
    int CHANNELS = 4;
    
    fp = fopen("/tmp/server.log", "a");
    if(fp == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    setvbuf(fp, NULL, _IOLBF, BUFSIZ); //flushes output on each print statement

    fprintf(fp, "%s: Starting server...\n", getTimeStamp());

    //create the semaphore
    fprintf(fp, "%s: Server creating Semaphore, entering CR\n", getTimeStamp());
    semaphore = createSemaphore(SHNAME);
    leaveCriticalRegion(semaphore); //we are in the CR when creating the semaphore

    //enter critical region
    fprintf(fp, "%s: Server entering critical region\n", getTimeStamp());
    enterCriticalRegion(semaphore);

    //set up main channel (channel 0)
    fprintf(fp, "%s: Creating shared memory block\n", getTimeStamp());
    shmFd = createSHM(SHNAME);
    fprintf(fp, "%s: Initialising main channel\n", getTimeStamp());
    shmData = initSHM(shmFd, &initData);

    fprintf(fp, "%s: Server leaving critical region\n", getTimeStamp());
    leaveCriticalRegion(semaphore); //leave CR

    fprintf(fp, "Shared Memory Area created\n");

    counter = 1;
    // assign channel numbers 
    // we are using shmData[0] for setting up the channels
    while(counter < CHANNELS)
    {
        counter++; //increment

        //wait for the ticket to be taken (i.e until critical region unlocks)
        while(1) {
            fprintf(fp, "%s: Server entering critical region, checking ticket\n", getTimeStamp());
            enterCriticalRegion(semaphore);

            if(shmData[0].isTaken == true) {
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
        fprintf(stderr, "Channel %d has been assigned\n", shmData[0].ticket);
        fprintf(fp, "Channel %d has been assigned\n", shmData[0].ticket);

        //ticket is taken, generate a new ticket
        shmData[0].ticket = counter;
        shmData[0].soldOut = false;
        shmData[0].isTaken = false;

        fprintf(fp, "%s: Server leaving critical region\n", getTimeStamp());
        leaveCriticalRegion(semaphore);
    }

    //issue tickets across the channels
    //create one ticket per channel
    enterCriticalRegion(semaphore);
    counter = 1;
    while(counter < CHANNELS) {
        shmData[counter].ticket = counter;
        shmData[counter].soldOut = false;
        shmData[counter].isTaken = false;
        counter++;
    }

    //tell the clients tickets are on sale
    shmData[0].soldOut = true;
    leaveCriticalRegion(semaphore);

    fprintf(stderr, "%s: Channels have been dispatched, selling tickets\n", getTimeStamp());

    //sell tickets
    counter = 0;
    channelCounter = 0;
    ticketNumber = 3;
    while(counter < MAX_TICKETS) {
        fprintf(fp, "%s: Checking all channels. Tickets Sold:%d\n", getTimeStamp(), counter);
        //check for taken tickets
        while(1) {
            channelCounter++;
            if(channelCounter == CHANNELS) {
                channelCounter = 1;
            }
            if(shmData[channelCounter].isTaken == true) {
                fprintf(fp, "%s: ticket %d\n", getTimeStamp(), shmData[channelCounter].ticket);
                //breaks when a ticket has been taken from any channel
                break;
            }
            sleep(1);
        }
        ticketNumber++;
        counter++;
        //ticket sold
        fprintf(stderr, "Ticket %d sold to channel %d\n", shmData[channelCounter].ticket, channelCounter);

        //generate a new ticket for this channel
        shmData[channelCounter].ticket = ticketNumber;
        shmData[channelCounter].isTaken = false;
        shmData[channelCounter].soldOut = false;

    }

    fprintf(fp, "%s: Server entering critical region, tickets sold out\n", getTimeStamp());
    enterCriticalRegion(semaphore);
    channelCounter = 0;
    while(channelCounter < CHANNELS) {
        shmData[channelCounter].ticket = channelCounter;
        shmData[channelCounter].soldOut = true;
        shmData[channelCounter].isTaken = false;
        channelCounter++;
    }
    fprintf(fp, "%s: Server leaving critical region, tickets sold out\n", getTimeStamp());
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