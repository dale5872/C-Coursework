/*
 * Author: Group 1
 * Adam Alexander Campbell
 * Dale Baker-Allan
 * Fraser Coupland
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> //to include sleep();

//Mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void * newThread() {
    pthread_t child_thread; // 0 to 4,294,967,295
    pid_t parent_pid;
    
    //get child thread and parent id
    child_thread = pthread_self();

    parent_pid = getpid();

    //print to output
    printf("Child Thread Started with Thread ID: %lu from Parent PID: %d\n", (unsigned long int)child_thread, parent_pid);
    pthread_mutex_unlock(&mutex); //unlock for main thread to take lock

    //main thread will run

    pthread_mutex_lock(&mutex); //waits for main thread to unlock the mutex
    printf("Child Thread Terminated. Thread ID: %lu. Back to Parent PID: %d\n", (unsigned long int)child_thread, parent_pid);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pid_t parent_pid;
    pthread_t child_pid;

    //get parent thread id
    parent_pid = getpid();
    printf("Parent Process with PID: %d\n", parent_pid);

    //lock mutex and create new thread. will unlock when ready to continue
    //this thead.
    pthread_mutex_lock(&mutex);
    pthread_create(&child_pid, NULL, newThread, NULL);

    sleep(10);
    //print main thread
    pthread_mutex_lock(&mutex);
    printf("Parent Process Finished. PID: %d\n", parent_pid);
    pthread_mutex_unlock(&mutex);

    //wait for child_pid to terminate
    pthread_join(child_pid, NULL);

}
