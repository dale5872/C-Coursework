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

#include "sem.h"

sem_t * createSemaphore(char *name) {
    sem_t * semaphore;

    sem_unlink(name);

    //get the semaphore
    semaphore = sem_open(name, O_RDWR | O_CREAT, 0777, 0);

    if(semaphore == SEM_FAILED) {
        perror("Failed to create semaphore");
        exit(1);
    }
    
    return semaphore;
}

sem_t * getSemaphore(char *name) {
    sem_t * semaphore;

    //get semaphore
    semaphore = sem_open(name, 0);

    if(semaphore == SEM_FAILED) {
        perror("Failed to get semaphore");
        exit(1);
    }

    return semaphore;
}

void enterCriticalRegion(sem_t *sem) {
    int lockCode;
    
    //lock the semaphore (i.e. enter the critical region)
    //also makes the process wait until the semaphore is freed so it can be locked
    lockCode = sem_wait(sem);

    if(lockCode == -1) {
        perror("Failed to enter the critical region.");
        exit(1);
    }
}

void leaveCriticalRegion(sem_t *sem) {
    int lockCode;

    //free the semaphore
    lockCode = sem_post(sem);

    if(lockCode == -1) {
        perror("Failed to leave the critical region");
        exit(1);
    }

}