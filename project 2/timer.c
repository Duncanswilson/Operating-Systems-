/*	#include <time.h>
	#include <stdio.h>
	#include <stdlib.h>

	 int main(int argc, char const *argv[])
	{
		/* code 
	clock_t start = clock(), diff;
	//ProcessIntenseFunction();
	sleep(1);  
	diff = clock() - start; 
	int msec = diff * 1000 / CLOCKS_PER_SEC;
	printf("Raw msec var %d\n", msec);
	printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
		return 0;
	}
*///////////////////////////////////////////////////////////////////////////////

#include <pthread.h>
#include <stdio.h>
 
/* call back function - inform the user the time has expired */
void timeout_cb()
{
    printf("=== your time is up ===\n");
}
 
/* Go to sleep for a period of seconds */
static void* g_start_timer(void *msecs)
{
    /* function pointer */
    void (*function_pointer)();
 
    unsigned int microseconds = *((int*) msecs); 

    /* cast the seconds passed in to int and
     * set this for the period to wait */
    
    printf("go to sleep for %d microseconds\n", microseconds);
     
    /* assign the address of the cb to the function pointer */
    function_pointer = timeout_cb;     
    usleep(microseconds*1000000);
    /* call the cb to inform the user time is out */
    (*function_pointer)();
     
    pthread_exit(NULL);
}
 
int main()
{
    pthread_t thread_id;
    int msecs = 2;
    int rc;
 
    rc =  pthread_create(&thread_id, NULL, g_start_timer, (void *) &msecs);
    if(rc)
    printf("=== Failed to create thread\n");
 
    pthread_join(thread_id, NULL);
 
    printf("=== End of Program - all threads in ===\n");
 
    return 0;
}