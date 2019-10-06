/**
 * Authored by:
 * Adam Alexander Campbell (H00248437)
 * Dale Baker-Allan (H00243682)
 * Fraser Coupland (H00243858)
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "stack.h"

static int mockUp = 42;
static int *stackTop = &mockUp;
static int stackSize = DEFAULT_STACK_SIZE;
static int stackPosition = 0;
static int *values;


/** @return - size of the stack
 * Returns the current stack size. The max stack size is 10
 * but this returns the current number of elements within the stack
 */
int getStackSize()
{
  return stackSize;
}

/**
 * @param size - size of the stack
 * Changes the stack size to a new size, ensuring no elements
 * are removed when resizing the stack
 */
void setStackSize( int size)
{
    if(stackPosition > size) {
        stackPosition = size;
        stackTop = &values[stackPosition];
    }
    values = (int*)realloc(values, size * sizeof(int));
    stackSize = size;
}

/**
 * deletes the stack by freeing the array 'values'
 */
void deleteStack()
{
    free(values);
}

/**
 * @return - returns the value of the head of the stack
 */
int top()
{
   return *stackTop;
}

/**
 * @param *val - pointer to the poped value
 * @return 1 for success, 0 for failure
 * @var stackPosition - used to keep track of the top of the stack's position in the array
 *
 * We want to remove the element in the array held by stackPosition, then update the pointers to
 * point to the value in stackPosition - 1
 * Set *val to the address stored in *stackPosition before changing it's pointer value
 */
int pop( int *val)
{
    if(stackPosition == -1) {
        *val = 0;
        return 1;
    }
    *val = *stackTop;
    stackPosition--;
    stackTop = &values[stackPosition];
    if(*stackTop == values[stackPosition]) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @param val - the value to be pushed into the stack
 * @return 1 for success, 0 for failure
 * @var stackPosition - used to keep track of the top of the stack's position in the array
 *
 * Pushes the value into the array, updates the pointers that track the top of the stack
 * and check if the pointer directs to the value pushed to the stack
 */
int push( int val)
{
    if(values == NULL) {
        values = (int*)malloc(stackSize * sizeof(int));
    }
    if(stackPosition == stackSize-1) {
        setStackSize(stackSize+1);
    }
    values[stackPosition] = val;
    stackTop = &values[stackPosition];
    stackPosition++;
    if(*stackTop == val) {
        return 1;
    } else {
        return 0;
    }
}
