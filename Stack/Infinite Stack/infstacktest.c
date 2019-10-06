#include <stdlib.h>
#include <stdio.h>
#include "infstack.h"

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
  printPush(42);
  for( i=1; i<15; i++) {
    printPush(i);
  }

  for( i=0; i<15; i++) {
    printPop();
  }

  printPush(42);
  printPop();

  printPush(42);
  printPop();

  deleteStack();
  return 0;
}
