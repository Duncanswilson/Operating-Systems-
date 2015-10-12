#include "structs&funcs.h"
#include "file_funcs.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

// both macro used in the logToBoth function 
#define both(fp, fmt, ...) printf (fmt, __VA_ARGS__); fprintf (fp, fmt, __VA_ARGS__)

////////////    PLEASE READ THE READ ME FILE FOR CLARIFICATION ON WHY THE TIMES ARE THE WAY THEY ARE AS WELL AS FURTHER NOTES  ///////////////
/////// Because I would  mark myself down if I got this code without any preface or reasoning for why the times are the way they are ////////

int runtTimeCalc(tasks *currentTask, config cycles); 
/*void logToMonitor(threadStruct threadParam, config cycles, processQueue *processQ);
void logToFile(FILE *filePointer, threadStruct threadParam, config cycles, processQueue *processQ);
void logToBoth(FILE *filePointer, threadStruct threadParam, config cycles, processQueue *processQ);*/
float timeCompare(struct timeval currentTime, struct timeval startTime);
void simulation(processQueue* processQ, config cycles, threadStruct threadParam);
void outputOp ( config cycles, char* inputString, float time, int processCounter);
void outputProcess ( config cycles, char* firstString, char*secondString, char* taskSpec, char*taskID, float time, int processCounter);
static void* ioTimer(void *currentData);
void shortestRunTime(processQueue *Q, config cycles);


int main(int argc, char* argv[]){
    //varible setup and initializtion  
    config cycles;  
    FILE *filePointer; 
    char *configFile;
    char  buffer[50];
    char errorCatch; 
    int fifoFlag = 0; 
    int charConvert;
    int cycleFlag; 
    int runTime = 0; 

    // a task buffer to hold the data from the meta-Data
    tasks *testTask = (tasks*)malloc(1*sizeof(tasks));

    // pcb is a data structure for storing important process data (specifically the task Queue)
    PCB* currentProcess = (PCB*)malloc(1*sizeof(PCB));

    // set up queue of processes
    processQueue *processQ = (processQueue*)malloc(1*sizeof(processQueue));
    createProcess(processQ);

    threadStruct threadParam;

////////////////////////////////// Begin Simulator Algorithm ///////////////////////////////////////
///////////////////////////////// File Input ///////////////////////////////////////////////////////
/* This section does two things: read in/store the config file and then takes the directory for
the meta data file and then reads in/stores the meta-data file */

//////////////////////////////// read in config ////////////////////////////////////    
//assign command-line args 
    configFile = argv[1];

//read in the config file 
    //open file 
    filePointer = fopen(configFile, "r"); 
        //catch errors
        if(filePointer == NULL){
            printf("No config file entered, aborting program\n");
            return 0;
        }

    //buffer out the first 3 lines
     fgets(buffer, 50, filePointer);    
     fgets(buffer, 50, filePointer);    

    // get meta-data directory 
     fgets(buffer, 12, filePointer);    
     fgetstr(cycles.metaPath, 100, filePointer);

    //get the scheduling type
     fgets(buffer, 17, filePointer);
     fgetstr(cycles.sched,100, filePointer);
     if(strcmp(cycles.sched,"FIFO") == 0){
     	fifoFlag = 1; 
     }

    //process time 
    cycles.processorCT = getCycle(filePointer); 
    //montior time 
    cycles.monitorCT = getCycle(filePointer);
    //HDD time 
    cycles.hddCT = getCycle(filePointer);
    //print time 
    cycles.printCT = getCycle(filePointer);
    //keyboard time 
    cycles.keyboardCT = getCycle(filePointer); 

    //log method 
     fgets(buffer, 13, filePointer);
     charConvert =  fgetc(filePointer);
     cycles.logMethod = (char) charConvert; 
     fgets(buffer, 16, filePointer);

    //log file path (file/monitior/both) 
    if (cycles.logMethod == 'F'||cycles.logMethod =='B'){
     fgets(buffer, 16, filePointer);
     fgetstr(cycles.filePath,100, filePointer);
    }
    //close file 
    fclose(filePointer); 
    //assign for thread paramter 
    threadParam.cycles = cycles; 

 //////////////////////////////// read in meta-data ////////////////////////////////////    
/* This section loads in the meta data from directory and if opened correctly 
 then it loops through the file loading each individual task into the process queue*/ 
    //read in meta data
    filePointer =  fopen(cycles.metaPath, "r");
        //catch errors
        if(filePointer == NULL){
            printf("Error with meta-data path, aborting program\n"); 
            return 0;
        }

    // meta-data read in error 
  	errorCatch = fgetc(filePointer);
  	if(errorCatch != 'S'){ 
  	  printf("Meta-data file not in expected format! Ending Simulation.\n");
  	  return 0;
  	}

  	//skip first line 
    fgets(buffer,100,filePointer);

    // prime the loop with the system start (DON'T ADD TO QUEUE!)
    cycleFlag = taskParse (testTask, &filePointer);

    //loop through adding each task into the PCB's taskQueue
    //these loops use the return from task parse to control for adding 
    //process to the queue, refernce the file_funcs.h file for more info 
    while(cycleFlag !=0){
        //set up current task queue
        runTime = 0; 
        taskQueue* taskQ = (taskQueue*)malloc(1*sizeof(taskQueue));
        createTask(taskQ);

        cycleFlag = taskParse (testTask, &filePointer);
        
        while(cycleFlag != 2 && cycleFlag != 0 && cycleFlag != 3){

         // if not fifo calculate runtime 
         if(fifoFlag == 0){
 	        runTime += runtTimeCalc(testTask, cycles);
 	     }
         enqTask(taskQ, testTask);

         // if the taskParse function returns a -1 then buffer out the newline 
         if(cycleFlag == -1){ 
         fgetc(filePointer); 
         }

         cycleFlag = taskParse (testTask, &filePointer);
        }
        currentProcess->processState = "ready";
        currentProcess->processID = runTime; 

        //printf("\nTask Queue and runTime %d: \n", currentProcess->processID);
        //displayT(currentProcess->taskQ);
        if(cycleFlag != 0 && cycleFlag != 3 ){
	        currentProcess->taskQ = *taskQ;
	        enqProcess(processQ, currentProcess);
	     }    
        free(taskQ);
        //reset flag for next process 
        if(cycleFlag == 2)
            cycleFlag = 1;
    }
   fclose(filePointer); 
   //displayP(processQ);


///////////////////////////////// Start Process 1 ////////////////////////////////////////////////////////////////////
 // This section seperates the rest of the program depending on the type of logging as specified in the config file //
 // If the logging method is to both or file then it handles the file opening in the if loop                        //
 	simulation(processQ, cycles, threadParam);
    return 0; 
}

///////////////// functions ////////////////////////////////////////////////////////////////////////////////
/* logToMonitor() - this function takes in:  -the threadStruct to be passed into the the thread function,
                                              -the config struct which contains the cycle numbers,
                                              -the queue of tasks read in from the meta-data file 
(More on all of the structures can be found in structs&funcs.h file :) ) 
As mentioned at the beginning of the file for more information my reasoning for using this timing method 
please read the readMe.txt file! 
*/ 


/* timeCompare() - a function which returns the foating point value 
                   of the current time minus the start time, due to the structure
                   of timeval you have to split the seconds and microseconds for
                   the accuracy we're looking for*/

float timeCompare(struct timeval currentTime, struct timeval startTime){
return ((float)(currentTime.tv_sec - startTime.tv_sec) +(currentTime.tv_usec - startTime.tv_usec)*.000001); 
}

/* ioTimer() -  This is the thread function which takes a threadStruct as a parameter (more on this data structure in structs&funcs.h)
                calculates the cycles time and millisecond time and then waits for the proper time frame using nanosleep function 
                (to learn why I used this function check the readme file!)  */
static void* ioTimer(void *currentData){
    threadStruct  *current = (threadStruct*)currentData;
    struct timespec time;
    int secondCtr = 0; 
    // a switch that calculates the task type of I/O operation 
    switch(current->currentTask.taskSpec[0]){
        case 'm':
            current->currentTime = current->cycles.monitorCT*current->currentTask.cycleNum;
            break;
        case 'h':
            current->currentTime = current->cycles.hddCT*current->currentTask.cycleNum;
            break;
        case 'p':
            current->currentTime = current->cycles.printCT*current->currentTask.cycleNum;
            break;
        case 'k':
            current->currentTime = current->cycles.keyboardCT*current->currentTask.cycleNum;
            break;
    } 
    // convert from milli to nano and pass in nanosecond parameter 
    //printf("this is the currentTime: %f\n", current->currentTime);
    while(current->currentTime >= 1000){
    	secondCtr++;
    	current->currentTime -= 1000;
    }
    time.tv_nsec =  current->currentTime*1000000;
    time.tv_sec = secondCtr;
    nanosleep(&time, NULL); 
    return NULL;
}

int runtTimeCalc(tasks *currentTask, config cycles){
	int runTime; 
    switch(currentTask->taskSpec[0]){
        case 'm':
            runTime = cycles.monitorCT*currentTask->cycleNum;
            break;
        case 'h':
            runTime = cycles.hddCT*currentTask->cycleNum;
            break;
        case 'p':
            runTime = cycles.printCT*currentTask->cycleNum;
            break;
        case 'k':
            runTime = cycles.keyboardCT*currentTask->cycleNum;
            break;
        case 's': 
        	runTime = 0; 
        	break;
        case 'e':
        	runTime = 0; 
        	break; 
    } 
    return runTime; 
}

void simulation(processQueue* processQ, config cycles, threadStruct threadParam){
	int stopFlag = 1; 
    int processCounter = 1; 
    int threadCatch; 
    pthread_t thread_id; 
    tasks currentTask; 
    struct timeval startTime;
    struct timeval currentTime; 
    gettimeofday(&startTime, NULL);
    struct timespec time;
    time.tv_sec = 0; 
    PCB currentProcess;

    // print the initial simulator display to file 
    outputOp(cycles, "- Simulator program starting", timeCompare(currentTime, startTime), ' ');
 /* loop through each program in the metadata file (for this project there is only one program)
 for each loop through it does the calculation and delay for the processor task
 for each I/O task it creates a thread in which the cycle calculation is done and then cancels 
 the thread
 After each of these respective computations/delays it then prints the time difference to the file 
 Once the program task is end, it sets the stopflag and exits (this will be changed as we extend the program) 
 */
    while(emptyProcess(processQ) != 0)
    {
    		currentProcess = deqProcess(processQ); 
		    gettimeofday(&currentTime, NULL);
		    currentProcess.processState = "Ready"; 
		    outputOp(cycles, "- OS: preparing process", timeCompare(currentTime, startTime), processCounter);
		    
		    gettimeofday(&currentTime, NULL);
		    currentProcess.processState = "Running"; 
		    outputOp(cycles, "- OS: starting process", timeCompare(currentTime, startTime), processCounter);

       while(emptyTask(&currentProcess.taskQ) != 0){

	        currentTask = deqTask(&(currentProcess.taskQ));
            gettimeofday(&currentTime, NULL);
            outputProcess(cycles, "- Process", ": start", currentTask.taskSpec, currentTask.taskID , timeCompare(currentTime, startTime), processCounter);

            if(strcmp(currentTask.taskID, "input") == 0 || strcmp(currentTask.taskID, "output") == 0) {
                //thread stuff
                threadParam.currentTask = currentTask; 
                threadCatch = pthread_create(&thread_id, NULL, ioTimer, (void*) &threadParam); 
                if(threadCatch)
                    printf("Error in thread creation.\n");
                else
                    pthread_join(thread_id, NULL); 
                pthread_cancel(thread_id);
            }

            else if(strcmp(currentTask.taskID, "action") == 0){
                    time.tv_nsec = cycles.processorCT*currentTask.cycleNum *1000000;
                    nanosleep(&time, NULL); 
            }
            gettimeofday(&currentTime, NULL);
            outputProcess(cycles, "- Process", ": end", currentTask.taskSpec, currentTask.taskID , timeCompare(currentTime, startTime), processCounter);
        }

	    gettimeofday(&currentTime, NULL);
	    currentProcess.processState = "Terminated"; 
	    outputOp(cycles, "- OS: removing process", timeCompare(currentTime, startTime), processCounter);
		processCounter++;
		if(strcmp(cycles.sched, "SRTF") == 0){ 	        
			shortestRunTime(processQ, cycles);
		}
	}

	gettimeofday(&currentTime, NULL);
	outputOp(cycles, "- Simulator program ending", timeCompare(currentTime, startTime), ' ');

}

void outputOp ( config cycles, char* inputString, float time, int processCounter)
{
    FILE* outputFile;

    // Check if only outputting to the monitor
    if (cycles.logMethod == 'M' ) 
    {
		printf( "%f %s %d \n", time, inputString, processCounter);
    }

    else{
    	// Open the logPath file in append mode so it doesn't overwrite the contents
	    outputFile = fopen(cycles.filePath, "a");
	    if (outputFile == NULL)
	    {
	    	printf("Error: config Log File Path has incorrect formating\n");
	    	return; 
	    }
	    // Check if only outputting to the file
	    if (cycles.logMethod == 'F')
	    {
	        // Output file contents with a precision of 6
			fprintf(outputFile, "%f %s %d \n",  time, inputString, processCounter);

	        // Close the file
	        fclose(outputFile);
	    }

	    // Check if logging to both
	    if (cycles.logMethod == 'B')
	    {
			printf( "%f %s %d \n", time, inputString, processCounter);

	        // Open the logPath file in append mode so it doesn't overwrite the contents
	        outputFile = fopen(cycles.filePath, "a");

	        // Output file contents with a precision of 6
			fprintf(outputFile, "%f %s %d \n", time, inputString, processCounter);

	        // Close the file
	        fclose(outputFile);
	    }
	}

}

void outputProcess ( config cycles, char* firstString, char*secondString, char* taskSpec, char*taskID, float time, int processCounter)
{
    FILE* outputFile;

    // Check if only outputting to the monitor
    if (cycles.logMethod == 'M' ) 
    {
		printf( "%f %s %d %s %s %s \n", time, firstString, processCounter, secondString, taskSpec, taskID);
    }

    else{
    	// Open the logPath file in append mode so it doesn't overwrite the contents
	    outputFile = fopen(cycles.filePath, "a");
	    if (outputFile == NULL)
	    {
	    	printf("Error: config Log File Path has incorrect formating\n");
	    	return; 
	    }
	    // Check if only outputting to the file
	    if (cycles.logMethod == 'F')
	    {
	        // Output file contents with a precision of 6
			fprintf(outputFile, "%f %s %d %s %s %s \n", time, firstString, processCounter, secondString, taskSpec, taskID);
	        // Close the file
	        fclose(outputFile);
	    }

	    // Check if logging to both
	    if (cycles.logMethod == 'B')
	    {
			printf( "%f %s %d %s %s %s \n", time, firstString, processCounter, secondString, taskSpec, taskID);

	        // Open the logPath file in append mode so it doesn't overwrite the contents
	        outputFile = fopen(cycles.filePath, "a");

	        // Output file contents with a precision of 6
			fprintf(outputFile, "%f %s %d %s %s %s \n", time, firstString, processCounter, secondString, taskSpec, taskID);
	        // Close the file
	        fclose(outputFile);
	    }
	}

}

void shortestRunTime(processQueue *Q, config cycles){
	int runTime = 0; 
	PCBnode *currentProcess = Q->front;
	taskNode *currentTask;
	PCB swapProcess;  
	while(currentProcess != NULL) {
		runTime = 0; 
		currentTask = currentProcess->process.taskQ.front; 

		while(currentTask != NULL){ 
			runTime += runtTimeCalc(&currentTask->task, cycles);
			currentTask = currentTask->ptr;	
		}

		if(currentProcess->process.processID != runTime){
			swapProcess = removeProcess(Q, currentProcess); 
			enqProcess(Q, &swapProcess); 
		}
		currentProcess = currentProcess->ptr;
	}
} 