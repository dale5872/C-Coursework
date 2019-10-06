#ifndef _sem_h_
#define _sem_h_

#include <semaphore.h>

extern sem_t * createSemaphore(char *name);
extern sem_t * getSemaphore(char *name);
extern void enterCriticalRegion(sem_t *sem);
extern void leaveCriticalRegionFree(sem_t *sem);

#endif