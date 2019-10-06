#ifndef _shm_h_
#define _shm_h_

#include <stdbool.h>

#define SHNAME "/shmserver" // shared memory
#define MAX_TICKETS 10
#define MAX_SLEEP 1 // seconds

typedef struct SHM {
    int ticket;
    bool isTaken;
    bool soldOut;
} SHMstruct;

extern char * getTimeStamp();

extern int createSHM(char *shname);
extern int loadSHM( char *shname);

extern SHMstruct* initSHM( int fd, SHMstruct *data);
extern SHMstruct * accessSHM(int fd);

extern void clearSHM(SHMstruct * shm);
extern void closeSHM(int fd);
extern void destroySHM(char * shname);

#endif
