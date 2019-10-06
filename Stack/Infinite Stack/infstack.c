/**
 * Authored by:
 * Adam Alexander Campbell (H00248437)
 * Dale Baker-Allan (H00243682)
 * Fraser Coupland (H00243858)
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "infstack.h"

struct stackNode {
  struct stackNode *nextNode;
  int value;
} stackNode;

static struct stackNode *head = NULL;
static int size = 0;

/**
 * @return size - size of the stack
 */
int getStackSize() {
    return size;
}

/**
 * loop through the linked list and free every node, starting with the head
 */
void deleteStack() {
    struct stackNode *temp;
    while(head != NULL) {
        temp = head;
        head = head->nextNode;
        free(head);
    }
}

/**
 * @return the top value of the stack (the value of the head node)
 */
int top() {
    if(head == NULL) {
        return 0;
    } else {
        return head->value;
    }
}

/**
 * @param val - value to be pushed into the stack
 * @return 1 for success, 0 for failure
 * Create a new stackNode struct and allocate memory for it
 * Set the new node as the head node and it's next node as the old head node
 * Check the new node has been added correctly
 */
int push (int val) {
    struct stackNode *newNode = (struct stackNode*)malloc(sizeof(struct stackNode));
    newNode->nextNode = head;
    newNode->value = val;
    head = newNode;

    //check assign properly
    if(head->value == val) {
        size++;
        return 1;
    } else {
        return 0;
    }
}

/**
 * @param val - int pointer referencing to the popped value
 * @return 1 for success, 0 for failure
 * Remove the top value of the stack and change the head pointer to the next
 * element in the stack
 * Check the element has been removed correctly
 */
int pop (int *val) {

    //check if head is null and size is 0, is true then assume the stack is empty and return a failure
    if(head == NULL && size == 0) {
        return 0;
    }
    int oldValue;
    *val = head->value;

    oldValue = head->value;
    head = head->nextNode;
    size--;

    //check the head's value is not the same and the head has been changed

    if(head == NULL && size == 0) {
        //we can assume the stack is empty by both the size being 0 and there being no head node
        return 1;
    } else if(head->value != oldValue) {
        return 1;
    } else {
        return 0;
    }
}
