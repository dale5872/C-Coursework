#include <stdlib.h>
#include <stdio.h>
#include "stack.h"
#include "stack.h"

void printState( )
{
  printf( "Size: %d; Top-element: %d.\n", getStackSize(), top());
}

void printPush( int val)
{
  int res;
  res = push(val);
  printf( "attempting push( %d)...", val);
  if( res ==0) {
    printf(" failed.\n");
  } else {
    printf(" succeeded.\n");
  }
  printState();
}

void printSetSize( int size)
{
  printf( "executing setStackSize( %d).\n", size);
  setStackSize( size);
  printState();
}

void printPop( )
{
  int val;
  int res;
  res = pop( &val);
  printf( "attempting pop( ...) ...");
  if( res ==0) {
    printf(" failed.\n");
  } else {
    printf(" yields %d.\n", val);
  }
  printState();
}

int main()
{
  int i;

  printState();
  for( i=1; i<15; i++) {
    printPush(i);
  }

  printSetSize( 9);
  printSetSize( 17);

  for( i=0; i<15; i++) {
    printPop();
  }

  printSetSize( 0);
  printPush(42);
  printPop();

  printSetSize( 1);
  printPush(42);
  printPop();

  deleteStack();

  return 0;
}


