#include <pthread.h>
#include "structs&funcs.h"
#include "file_funcs.h"
#include <time.h>
#include <unistd.h>

// both macro used in thw logToBoth function 
#define both(fp, fmt, ...) printf (fmt, __VA_ARGS__); fprintf (fp, fmt, __VA_ARGS__)

void logToMonitor(tasks currentTask, float currentTime, threadStruct threadParam, config cycles, Queue *processQ);
void logToFile(FILE *filePointer, tasks currentTask, float currentTime, threadStruct threadParam, config cycles, Queue *processQ);
void logToBoth(FILE *filePointer, tasks currentTask, float currentTime, threadStruct threadParam, config cycles, Queue *processQ);
static void* ioTimer(void *currentData);

int main(int argc, char* argv[]){
//varible setup 
	config cycles;  
	FILE *filePointer; 
	char *configFile;
	char  buffer[50];
	int charConvert;
	int cycleFlag; 
	float currentTime = 0;
	// task is a data structure storing each task in the processes metaData
	tasks *testTask = (tasks*)malloc(1*sizeof(tasks));
	tasks currentTask; 
    // set up queue of tasks
	Queue *processQ = (Queue*)malloc(1*sizeof(Queue));
	create(processQ);

	threadStruct threadParam;
////////////////////////////////// Begin Simulator Algorithm ///////////////////////////////////////
//printf("%f - Simulator program starting\n", (float)currentTime);
///////////////////////////////// File Input ///////////////////////////////////////////////////////
//printf("%f - OS: preparing process 1 \n", (float)currentTime );	
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
//read in meta data
 	filePointer =  fopen(cycles.metaPath, "r");
 	    //catch errors
		if(filePointer == NULL){
			printf("Error with meta-data path, aborting program\n"); 
			return 0;
		}
	//skip first line 
	fgets(buffer,100,filePointer);


	//then there isn't a period at the end of the line 
    cycleFlag = taskParse (testTask, &filePointer);
    enq(processQ, testTask);

   while(cycleFlag != 0){
     testTask = (tasks*)malloc(1*sizeof(tasks));
     cycleFlag = taskParse (testTask, &filePointer);
     enq(processQ, testTask);
     //add data to the queue here 
     if(cycleFlag == -1){ 
     fgetc(filePointer); 
     }
    }

   fclose(filePointer); 


///////////////////////////////// Start Process 1 //////////////////////////////////////////////////////
/*	printf("%f - OS: starting process 1 \n", currentTime );
	// dequeue the system start task 
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 
*/
    if(cycles.logMethod == 'M')
  	  logToMonitor(currentTask, currentTime, threadParam, cycles, processQ);

  	if(cycles.logMethod == 'F' || cycles.logMethod == 'B'){
  	 	filePointer = fopen(cycles.filePath, "w");

		if(filePointer == NULL){
			printf("Error in creating log file from directory, aborting program\n");
			return 0;
		 }
		if(cycles.logMethod == 'F')
  			logToFile(filePointer, currentTask, currentTime, threadParam, cycles, processQ); 
  		else
  			logToBoth(filePointer, currentTask, currentTime, threadParam, cycles, processQ);
  		fclose(filePointer);

  	}
	return 0; 
}





///////////////// functions ////////////////////////////////////////////////////////////////////////////////

void logToMonitor(tasks currentTask, float currentTime, threadStruct threadParam, config cycles, Queue *processQ){
	int stopFlag = 1; 
	int processCounter = 1; 
	int threadCatch; 
	pthread_t thread_id; 
	
	printf("%f - Simulator program starting\n", (float)currentTime);
	printf("%f - OS: preparing process 1 \n", (float)currentTime );	
	printf("%f - OS: starting process 1 \n", currentTime );

	// dequeue the system start task 
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 

	while(stopFlag == 1) 
		{

            if(strcmp(currentTask.taskID, "application") == 0)
				processCounter++; 

			printf("%f - Process %d: start %s %s \n", currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

            if(strcmp(currentTask.taskID, "input") == 0 || strcmp(currentTask.taskID, "output") == 0) {
				//thread stuff for timer
				//set up data 
				threadParam.currentTask = currentTask; 
				threadParam.currentTime = currentTime; 
				threadCatch = pthread_create(&thread_id, NULL, ioTimer, (void*) &threadParam); 
				if(threadCatch)
					printf("Error in thread creation.\n");
				else
					pthread_join(thread_id, NULL); 
				currentTime += threadParam.currentTime*.001;
			}

            else if(strcmp(currentTask.taskID, "action") == 0){
					usleep(cycles.processorCT*currentTask.cycleNum *1000); 
					currentTime += cycles.processorCT*currentTask.cycleNum*.001;
			}

			printf("%f - Process %d: end %s %s \n",  currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

			currentTask = *(deq(processQ)); 

            if(strcmp(currentTask.taskSpec ,"end") == 0)
				stopFlag = 0; 
		} 


		printf("%f - OS: removing process 1 \n", currentTime );
		printf("%f - Simulator program ending  \n", currentTime );
}

void logToFile(FILE* filePointer, tasks currentTask, float currentTime, threadStruct threadParam, config cycles, Queue *processQ){
	int stopFlag = 1; 
	int processCounter = 1; 
	int threadCatch; 
	pthread_t thread_id; 

	fprintf(filePointer, "%f - Simulator program starting\n", (float)currentTime);
	fprintf(filePointer, "%f - OS: preparing process 1 \n", (float)currentTime );	
	fprintf(filePointer, "%f - OS: starting process 1 \n", currentTime );

	// dequeue the system start task 
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 

	while(stopFlag == 1) 
		{

            if(strcmp(currentTask.taskID, "application") == 0)
				processCounter++; 

			fprintf(filePointer, "%f - Process %d: start %s %s \n", currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

            if(strcmp(currentTask.taskID, "input") == 0 || strcmp(currentTask.taskID, "output") == 0){
				//thread stuff for timer
				//set up data 
				threadParam.currentTask = currentTask; 
				threadParam.currentTime = currentTime; 
				threadCatch = pthread_create(&thread_id, NULL, ioTimer, (void*) &threadParam); 
				if(threadCatch)
					fprintf(filePointer, "Error in thread creation.\n");
				else
					pthread_join(thread_id, NULL); 
				currentTime += threadParam.currentTime*.001;
			}

            else if(strcmp(currentTask.taskID, "action") == 0){
					usleep(cycles.processorCT*currentTask.cycleNum *1000); 
					currentTime += cycles.processorCT*currentTask.cycleNum*.001;
			}

			fprintf(filePointer, "%f - Process %d: end %s %s \n",  currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

			currentTask = *(deq(processQ)); 

            if(strcmp(currentTask.taskSpec ,"end") == 0)
				stopFlag = 0; 
		} 


		fprintf(filePointer, "%f - OS: removing process 1 \n", currentTime );
		fprintf(filePointer, "%f - Simulator program ending  \n", currentTime );
}

void logToBoth(FILE * filePointer, tasks currentTask, float currentTime, threadStruct threadParam, config cycles, Queue *processQ){
	int stopFlag = 1; 
	int processCounter = 1; 
	int threadCatch; 
	pthread_t thread_id;
	
	both(filePointer, "%f - Simulator program starting\n", currentTime);
	both(filePointer,"%f - OS: preparing process 1 \n", currentTime );	
	both(filePointer,"%f - OS: starting process 1 \n", currentTime );

	// dequeue the system start task 
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 

	while(stopFlag == 1) 
		{

            if(strcmp(currentTask.taskID, "application") == 0)
				processCounter++; 

			both(filePointer,"%f - Process %d: start %s %s \n", currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

            if(strcmp(currentTask.taskID, "input") == 0 || strcmp(currentTask.taskID, "output") == 0) {
				//thread stuff for timer
				//set up data 
				threadParam.currentTask = currentTask; 
				threadParam.currentTime = currentTime; 
				threadCatch = pthread_create(&thread_id, NULL, ioTimer, (void*) &threadParam); 
				if(threadCatch){
					printf("Error in thread creation.\n");
				}
				else
					pthread_join(thread_id, NULL); 
					currentTime += threadParam.currentTime*.001;
			}

            else if(strcmp(currentTask.taskID, "action") == 0){
					usleep(cycles.processorCT*currentTask.cycleNum *1000); 
					currentTime += cycles.processorCT*currentTask.cycleNum*.001;
			}

			both(filePointer,"%f - Process %d: end %s %s \n",  currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

			currentTask = *(deq(processQ)); 

            if(strcmp(currentTask.taskSpec ,"end") == 0)
				stopFlag = 0; 
		} 


		both(filePointer,"%f - OS: removing process 1 \n", currentTime );
		both(filePointer,"%f - Simulator program ending  \n", currentTime );
}

static void* ioTimer(void *currentData){
	threadStruct  *current = (threadStruct*)currentData;
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
	// convert from milliseconds to microseconds and pass in microsecond parameter 
	//printf("Current waitTime %f\n", current.currentTime);
	useconds_t usecs =  current->currentTime*1000;
	usleep(usecs); 
	return NULL;
}
