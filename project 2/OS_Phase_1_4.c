#include "structs&funcs.h"
#include "file_funcs.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

// both macro used in thw logToBoth function 
#define both(fp, fmt, ...) printf (fmt, __VA_ARGS__); fprintf (fp, fmt, __VA_ARGS__)

////////////    PLEASE READ THE READ ME FILE FOR CLARIFICATION ON WHY THE TIMES ARE THE WAY THEY ARE AS WELL AS FURTHER NOTES  ///////////////
/////// Because I would  mark myself down if I got this code without any preface or reasoning for why the times are the way they are ////////

void logToMonitor(threadStruct threadParam, config cycles, Queue *processQ);
void logToFile(FILE *filePointer, threadStruct threadParam, config cycles, Queue *processQ);
void logToBoth(FILE *filePointer, threadStruct threadParam, config cycles, Queue *processQ);
float timeCompare(struct timeval currentTime, struct timeval startTime);
static void* ioTimer(void *currentData);


int main(int argc, char* argv[]){
    //varible setup and initializtion  
    config cycles;  
    FILE *filePointer; 
    char *configFile;
    char  buffer[50];
    int charConvert;
    int cycleFlag; 

    // task is a data structure storing each task in the processes metaData
    tasks *testTask = (tasks*)malloc(1*sizeof(tasks));

    // set up queue of tasks
    Queue *processQ = (Queue*)malloc(1*sizeof(Queue));
    create(processQ);

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
 then it loops through the file loading each indivual task into the process queue*/ 
    //read in meta data
    filePointer =  fopen(cycles.metaPath, "r");
        //catch errors
        if(filePointer == NULL){
            printf("Error with meta-data path, aborting program\n"); 
            return 0;
        }
    //skip first line 
    fgets(buffer,100,filePointer);


    // prime the loop with the first enqueue
    cycleFlag = taskParse (testTask, &filePointer);
    enq(processQ, testTask);

    //loop through adding each task into the process queue until EOF
    while(cycleFlag != 0){
     
     testTask = (tasks*)malloc(1*sizeof(tasks));
     cycleFlag = taskParse (testTask, &filePointer);
     enq(processQ, testTask);

     // if the taskParse function returns a -1 then buffer out the newline 
     if(cycleFlag == -1){ 
     fgetc(filePointer); 
     }
    }

   fclose(filePointer); 


///////////////////////////////// Start Process 1 ////////////////////////////////////////////////////////////////////
 // This section seperates the rest of the program depending on the type of logging as specified in the config file //
 // If the logging method is to both of file then it handles the file opening in the if loop                        //

    if(cycles.logMethod == 'M')
      logToMonitor(threadParam, cycles, processQ);

    if(cycles.logMethod == 'F' || cycles.logMethod == 'B'){
        filePointer = fopen(cycles.filePath, "w");

        if(filePointer == NULL){
            printf("Error in creating log file from directory, aborting program\n");
            return 0;
         }
        if(cycles.logMethod == 'F')
            logToFile(filePointer, threadParam, cycles, processQ); 
        else
            logToBoth(filePointer, threadParam, cycles, processQ);
        fclose(filePointer);

    }
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

void logToMonitor(threadStruct threadParam, config cycles, Queue *processQ){
    // variable decleration and initial assignment of times for later calculation 
    int stopFlag = 1; 
    int processCounter = 1; 
    int threadCatch; 
    tasks currentTask;
    pthread_t thread_id; 
    struct timeval currentTime, startTime; 
    gettimeofday(&startTime, NULL);
    struct timespec time;
    time.tv_sec = 0;
    PCB *controlBlock = malloc(1*sizeof(PCB)); //this will eventually become an array or other structure 

    // print the initial simulator display and change process state 
    printf("%f - Simulator program starting\n", timeCompare(startTime, startTime));
    gettimeofday(&currentTime, NULL);
    controlBlock->processState = "ready";
    printf("%f - OS: preparing process 1 \n", timeCompare(currentTime, startTime)); 
    gettimeofday(&currentTime, NULL);
    controlBlock->processState = "running";
    printf("%f - OS: starting process 1 \n", timeCompare(currentTime, startTime));
    gettimeofday(&currentTime, NULL);


    //clear the queue of the inital tasks which don't correlate with printed data 
    // (this will be changed as we extend the simulator)
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 

    /* loop through each program in the metadata file (for this project there is only one program)
     for each loop through it does the calculation and delay for the processor task
     for each I/O task it creates a thread in which the cycle calculation is done and then cancels 
     the thread
     After each of these respective computations/delays it then prints the time difference to the screen 
     Once the program task is end, it sets the stopflag and exits (this will be changed as we extend the program) 
     */
    while(stopFlag == 1) 
        {

            if(strcmp(currentTask.taskID, "application") == 0)
                processCounter++; 

            gettimeofday(&currentTime, NULL);
            printf("%f - Process %d: start %s %s \n", timeCompare(currentTime, startTime), processCounter, currentTask.taskSpec, currentTask.taskID);

            // thread stuff
            if(strcmp(currentTask.taskID, "input") == 0 || strcmp(currentTask.taskID, "output") == 0){
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
            printf("%f - Process %d: end %s %s \n",  timeCompare(currentTime, startTime), processCounter, currentTask.taskSpec, currentTask.taskID);

            currentTask = *(deq(processQ)); 

            if(strcmp(currentTask.taskSpec ,"end") == 0)
                stopFlag = 0; 
        } 

        gettimeofday(&currentTime, NULL);
        controlBlock->processState = "Terminated"; 
        printf("%f - OS: removing process 1 \n", timeCompare(currentTime, startTime));
        gettimeofday(&currentTime, NULL);
        printf("%f - Simulator program ending  \n", timeCompare(currentTime, startTime));
}


/* logToFile() - this function takes in:  -the file pointer predesignated to write to the file 
                                              -the threadStruct to be passed into the the thread function,
                                              -the config struct which contains the cycle numbers,
                                              -the queue of tasks read in from the meta-data file 
(More on all of the structures can be found in structs&funcs.h file :) )
As mentioned at the beginning of the file for more information my reasoning for using this timing method 
please read the readMe.txt file!  
*/
void logToFile(FILE* filePointer, threadStruct threadParam, config cycles, Queue *processQ){
    // variable decleration and initial assignment of times for later calculation 
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
    PCB *controlBlock = malloc(1*sizeof(PCB)); //this will eventually become an array or other structure 


    // print the initial simulator display to file 
    fprintf(filePointer, "%f - Simulator program starting\n", timeCompare(startTime, startTime));
    gettimeofday(&currentTime, NULL);
    controlBlock->processState = "Ready"; 
    fprintf(filePointer, "%f - OS: preparing process 1 \n", timeCompare(currentTime, startTime));
    gettimeofday(&currentTime, NULL);
    controlBlock->processState = "Running"; 
    fprintf(filePointer, "%f - OS: starting process 1 \n", timeCompare(currentTime, startTime));

    //clear the queue of the inital tasks which don't correlate with printed data 
    // (this will be changed as we extend the simulator)
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 

    /* loop through each program in the metadata file (for this project there is only one program)
     for each loop through it does the calculation and delay for the processor task
     for each I/O task it creates a thread in which the cycle calculation is done and then cancels 
     the thread
     After each of these respective computations/delays it then prints the time difference to the file 
     Once the program task is end, it sets the stopflag and exits (this will be changed as we extend the program) 
     */
    while(stopFlag == 1) 
        {

            if(strcmp(currentTask.taskID, "application") == 0)
                processCounter++; 
            gettimeofday(&currentTime, NULL);
            fprintf(filePointer, "%f - Process %d: start %s %s \n", timeCompare(currentTime, startTime), processCounter, currentTask.taskSpec, currentTask.taskID);

            if(strcmp(currentTask.taskID, "input") == 0 || strcmp(currentTask.taskID, "output") == 0) {
                //thread stuff
                threadParam.currentTask = currentTask; 
                threadCatch = pthread_create(&thread_id, NULL, ioTimer, (void*) &threadParam); 
                if(threadCatch)
                    fprintf(filePointer, "Error in thread creation.\n");
                else
                    pthread_join(thread_id, NULL); 
                pthread_cancel(thread_id);
            }

            else if(strcmp(currentTask.taskID, "action") == 0){
                    time.tv_nsec = cycles.processorCT*currentTask.cycleNum *1000000;
                    nanosleep(&time, NULL); 
            }
            gettimeofday(&currentTime, NULL);
            fprintf(filePointer, "%f - Process %d: end %s %s \n",  timeCompare(currentTime, startTime), processCounter, currentTask.taskSpec, currentTask.taskID);

            currentTask = *(deq(processQ)); 

            if(strcmp(currentTask.taskSpec ,"end") == 0)
                stopFlag = 0; 
        } 

        gettimeofday(&currentTime, NULL);
        controlBlock->processState = "Terminated"; 
        fprintf(filePointer, "%f - OS: removing process 1 \n", timeCompare(currentTime, startTime));
        gettimeofday(&currentTime, NULL);
        fprintf(filePointer, "%f - Simulator program ending  \n", timeCompare(currentTime, startTime));
}


/* logToFile() - this function takes in:  -the file pointer predesignated to write to the file 
                                              -the threadStruct to be passed into the the thread function,
                                              -the config struct which contains the cycle numbers,
                                              -the queue of tasks read in from the meta-data file 
(More on all of the structures can be found in structs&funcs.h file :) ) 
As mentioned at the beginning of the file for more information my reasoning for using this timing method 
please read the readMe.txt file! 
*Specific note on the both macro found in this function, it's a bundeling macro which writes both to
 the file and to the monitor (the code found in beginning of this file) 
*/ 

void logToBoth(FILE * filePointer, threadStruct threadParam, config cycles, Queue *processQ){
    // variable decleration and initial assignment of times for later calculation 
    int stopFlag = 1; 
    int processCounter = 1; 
    int threadCatch; 
    pthread_t thread_id;
    struct timeval currentTime; 
    struct timeval startTime;
    gettimeofday(&startTime, NULL);
    struct timespec time;
    time.tv_sec = 0; 
    tasks currentTask;
    PCB *controlBlock = malloc(1*sizeof(PCB));

    // print the initial simulator display both to file and montior  
    both(filePointer, "%f - Simulator program starting\n", timeCompare(startTime, startTime));
    gettimeofday(&currentTime, NULL);
    controlBlock->processState = "Ready"; 
    both(filePointer,"%f - OS: preparing process 1 \n", timeCompare(currentTime, startTime));   
    gettimeofday(&currentTime, NULL);
    controlBlock->processState = "Running"; 
    both(filePointer,"%f - OS: starting process 1 \n", timeCompare(currentTime, startTime));

    //clear the queue of the inital tasks which don't correlate with printed data 
    // (this will be changed as we extend the simulator)
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 
    currentTask = *(deq(processQ)); 

    /* loop through each program in the metadata file (for this project there is only one program)
     for each loop through it does the calculation and delay for the processor task
     for each I/O task it creates a thread in which the cycle calculation is done and then cancels 
     the thread
     After each of these respective computations/delays it then prints the time difference to the file and montior (using both macro)
     Once the program task is end, it sets the stopflag and exits (this will be changed as we extend the program) 
     */
    while(stopFlag == 1) 
        {

            if(strcmp(currentTask.taskID, "application") == 0)
                processCounter++; 
            gettimeofday(&currentTime, NULL);
            both(filePointer,"%f - Process %d: start %s %s \n", timeCompare(currentTime, startTime), processCounter, currentTask.taskSpec, currentTask.taskID);

            if(strcmp(currentTask.taskID, "input") == 0 || strcmp(currentTask.taskID, "output") == 0){
                //thread stuff
                threadParam.currentTask = currentTask; 
                threadCatch = pthread_create(&thread_id, NULL, ioTimer, (void*) &threadParam); 
                if(threadCatch){
                    printf("Error in thread creation.\n");
                }
                else
                    pthread_join(thread_id, NULL); 
                pthread_cancel(thread_id);
            }

            else if(strcmp(currentTask.taskID, "action") == 0){
                    time.tv_nsec = cycles.processorCT*currentTask.cycleNum *1000000;
                    nanosleep(&time, NULL); 
            }
            gettimeofday(&currentTime, NULL);
            both(filePointer,"%f - Process %d: end %s %s \n",  timeCompare(currentTime, startTime), processCounter, currentTask.taskSpec, currentTask.taskID);
 
            currentTask = *(deq(processQ)); 

            if(strcmp(currentTask.taskSpec ,"end") == 0)
                stopFlag = 0; 
        } 

        gettimeofday(&currentTime, NULL);
        controlBlock->processState = "Terminated"; 
        both(filePointer,"%f - OS: removing process 1 \n", timeCompare(currentTime, startTime));
        gettimeofday(&currentTime, NULL);
        both(filePointer,"%f - Simulator program ending  \n", timeCompare(currentTime, startTime));
}

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
    time.tv_nsec =  current->currentTime*1000000;
    time.tv_sec = 0;
    nanosleep(&time, NULL); 
    return NULL;
}