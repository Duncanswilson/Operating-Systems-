#include <pthread.h>
#include "structs&funcs.h"
#include <time.h>
#include <unistd.h>
//create process block data strucutre 
//
//
//
int getCycle(FILE *filePointer, char *cycleBuffer);
int taskParse(tasks *currentTask, FILE **filePointer);
void logToMonitor(tasks currentTask, float currentTime, threadStruct threadParam, config cycles, Queue *processQ);
void logToFile(FILE *filePointer, tasks currentTask, float currentTime, threadStruct threadParam, config cycles, Queue *processQ);
void logToBoth(FILE *filePointer, tasks currentTask, float currentTime, threadStruct threadParam, config cycles, Queue *processQ);



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
}


int main(int argc, char* argv[]){
//varible setup 
	config cycles;  
	FILE *filePointer; 
	char *configFile;
	char *metaDataFile;
	char  buffer[50];
	char  cycleBuffer[30]; 
	int fileInc = 0; 
	int charConvert;
	int cycleFlag; 
	float currentTime = 0;
	// task is a data structure storing each task in the processes metaData
	tasks *testTask = (tasks*)malloc(1*sizeof(tasks));
	tasks currentTask; 
    // set up queue of tasks
	Queue *processQ = (Queue*)malloc(1*sizeof(Queue));
	create(processQ);
    // zero out array
	for(fileInc = 0; fileInc < 30; fileInc++){
		cycleBuffer[fileInc] = 0; 
	} 
	threadStruct threadParam;
////////////////////////////////// Begin Simulator Algorithm ///////////////////////////////////////
//printf("%f - Simulator program starting\n", (float)currentTime);
///////////////////////////////// File Input ///////////////////////////////////////////////////////
//printf("%f - OS: preparing process 1 \n", (float)currentTime );	
//assign command-line args 
	configFile = argv[1];
	metaDataFile = argv[2];

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
	cycles.processorCT = getCycle(filePointer,cycleBuffer); 
	//montior time 
	cycles.monitorCT = getCycle(filePointer,cycleBuffer);
	//HDD time 
	cycles.hddCT = getCycle(filePointer,cycleBuffer);
	//print time 
	cycles.printCT = getCycle(filePointer, cycleBuffer);
	//keyboard time 
	cycles.keyboardCT = getCycle(filePointer, cycleBuffer); 

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
int getCycle(FILE *filePointer, char *cycleBuffer){
	 int test; 
	 test = fgetc(filePointer);
	 while ((char)test != ':'){
     test = fgetc(filePointer);
	 }
	//get space out 
	 int fileInc = 0;
     test = fgetc(filePointer);
     while((char)test != '\n'){
     test = fgetc(filePointer);
     cycleBuffer[fileInc] = (char)test; 
     fileInc++;
     }
     test =  atoi(cycleBuffer);
    return atoi(cycleBuffer);
}

int taskParse(tasks *currentTask, FILE **filePointer){
    char buffer; 
    char cycleNum[5];
    int iterator = 0;
    char taskID, taskSpec; 
// set the task ID via a switch from char to string for printing ease 
	for(iterator = 0; iterator < 6; iterator++){
		cycleNum[iterator] = 'n'; 
	}
	iterator = 0;
	taskID = fgetc(*filePointer);
	switch(taskID){
		case 'S':
			(*currentTask).taskID = "system"; 
			break;

		case 'P':
			(*currentTask).taskID = "action"; 
			break;

		case 'A': 
			(*currentTask).taskID = "application"; 
			break;

		case 'I':
			(*currentTask).taskID = "input"; 
			break; 

		case 'O':
			(*currentTask).taskID = "output"; 
			break; 
	}

	buffer = fgetc(*filePointer);

	taskSpec = fgetc(*filePointer);
	switch(taskSpec){
		case 's':
			(*currentTask).taskSpec = "start"; 
			break;

		case 'r': 
			(*currentTask).taskSpec = "processing"; 
			break;

		case 'h':
			(*currentTask).taskSpec = "hard drive"; 
			break; 

		case 'm':
			(*currentTask).taskSpec = "montior"; 
			break; 

		case 'k':
			(*currentTask).taskSpec = "keyboard"; 
			break;

		case 'p':
			(*currentTask).taskSpec = "printer"; 
			break;  

		case 'e':
			(*currentTask).taskSpec = "end"; 
			break;  
	}
	while((char)buffer != ')'){
     buffer = fgetc(*filePointer); 
	}
   
    buffer = fgetc(*filePointer); 
	while((char)buffer != ';' && (char)buffer != '.'){  
     cycleNum[iterator] = buffer; 
     iterator++; 
     buffer = fgetc(*filePointer); 
     }
     (*currentTask).cycleNum = atoi(cycleNum);
     
    //set up the return statement that will tell if last input *period*
    if((char)buffer == ';') {
     fgetc(*filePointer);
     if((char)fpeek(*filePointer) == '\n'){
     	return -1; 
     }
     	return 1; 
     }

    else{ 

    	return 0; 
     }
 }

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

			if(currentTask.taskID == "application")
				processCounter++; 

			printf("%f - Process %d: start %s %s \n", currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

			if(currentTask.taskID == "input" || currentTask.taskID == "output"){
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

			else if(currentTask.taskID == "action"){
					usleep(cycles.processorCT*currentTask.cycleNum *1000); 
					currentTime += cycles.processorCT*currentTask.cycleNum*.001;
			}

			printf("%f - Process %d: end %s %s \n",  currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

			currentTask = *(deq(processQ)); 

			if(currentTask.taskSpec == "end")
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

			if(currentTask.taskID == "application")
				processCounter++; 

			fprintf(filePointer, "%f - Process %d: start %s %s \n", currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

			if(currentTask.taskID == "input" || currentTask.taskID == "output"){
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

			else if(currentTask.taskID == "action"){
					usleep(cycles.processorCT*currentTask.cycleNum *1000); 
					currentTime += cycles.processorCT*currentTask.cycleNum*.001;
			}

			fprintf(filePointer, "%f - Process %d: end %s %s \n",  currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

			currentTask = *(deq(processQ)); 

			if(currentTask.taskSpec == "end")
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

			if(currentTask.taskID == "application")
				processCounter++; 

			both(filePointer,"%f - Process %d: start %s %s \n", currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

			if(currentTask.taskID == "input" || currentTask.taskID == "output"){
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

			else if(currentTask.taskID == "action"){
					usleep(cycles.processorCT*currentTask.cycleNum *1000); 
					currentTime += cycles.processorCT*currentTask.cycleNum*.001;
			}

			both(filePointer,"%f - Process %d: end %s %s \n",  currentTime, processCounter, currentTask.taskSpec, currentTask.taskID);

			currentTask = *(deq(processQ)); 

			if(currentTask.taskSpec == "end")
				stopFlag = 0; 
		} 


		both(filePointer,"%f - OS: removing process 1 \n", currentTime );
		both(filePointer,"%f - Simulator program ending  \n", currentTime );
}

