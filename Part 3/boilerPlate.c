#include "structs&funcs.h"
#include "file_funcs.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
 
////// PLEASE READ THE READ ME FILE FOR CLARIFICATION ON WHY THE TIMES ARE THE WAY THEY ARE AS WELL AS FURTHER NOTES  ///////////////
             ///// I would  mark myself down if I got this code without any preface or methodology ////////

int runtTimeCalc(tasks *currentTask, config cycles); 
float timeCompare(struct timespec currentTime, struct timespec startTime);
void simulation(processQueue* processQ, config cycles, threadStruct threadParam);
void outputOp (char* inputString);
void outputProcess ( config cycles, char* firstString, char*secondString, char* taskSpec, char*taskID, float time, int processCounter);
static void* ioTimer(void *currentData);
void shortestRunTime(processQueue *Q, config cycles);
void printerFunct(struct timespec currentTime, struct timespec startTime, char* str1, char* str2, PCB currentProcess, int processCounter, config cycles);



int main(){

	outputOp("- OS: process %d to Blocked");
	return 0; 
}


void outputOp ( char* inputString){
  
        printf( "%s \n", inputString, 2);

}
                //else{
                //    pthread_join(thread_id, NULL); 
                //}
                //end thread 
                //pthread_cancel(thread_id);