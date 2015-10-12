#include <pthread.h>
#include <stdio.h>
#define NUM_THREADS     5

int global; 

void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}
void *PrintHelloWait(void *threadid)
{
   long tid;
   tid = (long)threadid;
   global = 1; 
   printf("Waiting on thread #%ld \n", tid);
   if(tid % 2 == 0)
   usleep(1000000);
   printf("Hello World! It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   global  = 0;  
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;
   for(t=0; t<NUM_THREADS; t++){
      printf("In main: creating thread %ld\n", t);
//      if(t % 2 == 0 )
         rc = pthread_create(&threads[t], NULL, PrintHelloWait, (void *)t);

      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }
   /* Last thing that main() should do */
   pthread_exit(NULL);
}