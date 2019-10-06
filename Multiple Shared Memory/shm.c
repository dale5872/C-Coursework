#include <stdbool.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "shm.h"

/* Generate a human-readable timestamp */
char * getTimeStamp() {
    time_t ltime = time(NULL);
    return strtok(ctime(&ltime), "\n");
}

/* Create Shared Memory Segment
 *
 * Function creates a named SHM file descriptor on the filesystem.
 *
 * @param shname Name of SHM
 * @return file descriptor
 */
int createSHM(char * shname)
{
    int shm;
    //open the shared memory
    shm = shm_open(shname, O_CREAT | O_RDWR, 0777);
    if(shm == -1) {
        //failed
        perror("Failed to create shared memory\n");
        exit(1);
    }

    return shm;
}

/* Load Shared Memory
 *
 * Function loads an existing named SHM, or gracefully fails
 * when the SHM does not exist.
 *
 * @param shname Name of SHM region
 * @return file descriptor
 */
int loadSHM(char * shname)
{
    int shm;

    //open the shared memory, only need O_RDWR as the block should exist
    shm = shm_open(shname, O_RDWR, 0);
    if(shm == -1) {
        perror("Failed to open shared memory block\n");
        exit(1);
    }

    return shm;
}

/* Access Existing SHM
 *
 * From an existing SHM file descriptor, allocate the SHMstruct and
 * return its pointer.
 *
 * @param fd File descriptor of existing SHM
 * @return Pointer to SHMstruct
 */
SHMstruct * accessSHM(int fd) {
    //SHMstruct newTicket = {0, false, false};
    int struct_size;
    SHMstruct * ptr;

    //we want to set the size of the shared memory block
    struct_size = 3 * sizeof(SHMstruct); //get the full size of the shared memory block
    ftruncate(fd, struct_size);

    //memory map the object
    ptr = (SHMstruct*) mmap(NULL, struct_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    //create a new SHMstruct
    //memcpy(ptr, &newTicket, struct_size);

    //return the pointer
    return ptr;
}

/* Initialise SHM
 *
 * From an existing SHM file descriptor, initilise the SHMstruct variable.
 *
 * HINT: use accessSHM()
 *
 * @param fd File descriptor of SHM
 * @return Pointer to SHMstruct
 */
SHMstruct * initSHM(int fd, SHMstruct *data) {
    SHMstruct * shm;

    //get a new struct
    shm = accessSHM(fd);

    //copy the data to the main channel (channel 0)
    shm[0].ticket = data->ticket;
    shm[0].isTaken = data->isTaken;
    shm[0].soldOut = data->soldOut;

    //return the pointer
    return shm;
}

/* De-allocate SHMstruct
 *
 * Function de-allocates an already existing SHMstruct.
 *
 * @param shm Pointer to SHMstruct
 */
void clearSHM(SHMstruct * shm)
{
    //remove the mapping of the shm object
    munmap(shm, sizeof(SHMstruct));
}

/* Close SHM file descriptor
 *
 * Function closes an existing SHM file descriptor.
 *
 * @param fd File descriptor of SHM
 */
void closeSHM(int fd)
{
    close(fd);
}

/* Unlink SHM
 *
 * Function destroys an existing SHM assuming that its
 * allocated memory has been de-allocated.
 *
 * @param shname Name of SHM
 */
void destroySHM(char * shname)
{
    shm_unlink(shname);
}
