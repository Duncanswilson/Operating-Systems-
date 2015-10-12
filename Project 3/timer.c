/*
 * This program calculates the time required to
 * execute the program specified as its first argument.
 * The time is printed in seconds, on standard out.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1E9

int main( int argc, char **argv )
    { 
  int aInt = 368;
  char str[15];
  sprintf(str, "%d", aInt);
  printf("This is the string!:  %s\n", str);
    return 0;   
  }