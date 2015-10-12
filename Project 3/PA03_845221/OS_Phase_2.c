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
void outputOp ( config cycles, char* inputString, float time, int processCounter);
void outputProcess ( config cycles, char* firstString, char*secondString, char* taskSpec, char*taskID, float time, int processCounter);
static void* ioTimer(void *currentData);
void shortestRunTime(processQueue *Q, config cycles);
void printerFunct(struct timespec currentTime, struct timespec startTime, char* str1, char* str2, PCB currentProcess, int processCounter, config cycles);


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
/*  This section loads in the meta data from directory and if opened correctly 
    then it loops through the file loading each process into the process queue 
    and for each process it loads each individual task into the PCB task queue
 */ 
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

    // prime the loop with the system start 
    cycleFlag = taskParse (testTask, &filePointer);

    /* At this point you need to know that: 
        TaskParse returns an integer with 5 meaningful values: 
            -1 - get the newline 
             0 - end of file (denoted by a period in meta-data)
             1 - normal task read-in 
             2 - end of current process flag (don't add to queue)
             3 - end of system simulator (don't add to queue)
     */
    //loop until end of meta-data file
    while(cycleFlag !=0){
        //set up current task queue
        runTime = 0; 
        taskQueue* taskQ = (taskQueue*)malloc(1*sizeof(taskQueue));
        createTask(taskQ);

        //prime the task loading loop 
        cycleFlag = taskParse (testTask, &filePointer);
        
        //loop until end of process (cycleFlag = 2 or 3 ) or end of file ( cycleFlag = 0 )
        while(cycleFlag != 2 && cycleFlag != 0 && cycleFlag != 3){

         // if not fifo calculate run-time 
         if(fifoFlag == 0){
            runTime += runtTimeCalc(testTask, cycles);
         }

         //loaf the current task into the task queue
         enqTask(taskQ, testTask);

         // if cycleFlag =  -1 then buffer out the newline 
         if(cycleFlag == -1){ 
         fgetc(filePointer); 
         }

         //get the next task 
         cycleFlag = taskParse (testTask, &filePointer);
        }

        //set task state to ready and load run-time  
        currentProcess->processState = "ready";
        currentProcess->processID = runTime; 

        // if it is the data is "load-worthy", then enqueue it 
        if(cycleFlag != 0 && cycleFlag != 3 ){
            currentProcess->taskQ = *taskQ;
            enqProcess(processQ, currentProcess);
         }    

        // deallocate the taskQ memory  
        free(taskQ);

        //reset flag for next process 
        if(cycleFlag == 2)
            cycleFlag = 1;
    }

    //close the  file connection 
   fclose(filePointer); 


///////////////////////////////// Start Simulation ////////////////////////////////////////////////////////////////////

    simulation(processQ, cycles, threadParam);
    return 0; 
}

///////////////// functions ///////////////////////////////////////////////////////////////////////////////////////////


/* timeCompare() - a function which returns the foating point value 
                   of the current time minus the start time, due to the structure
                   of timespec you have to split the seconds and nanoseconds for
                   the accuracy we're looking for
*/
float timeCompare(struct timespec stop, struct timespec start){
return ((float)( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec )/1E9); 
}

/* ioTimer() -  This is the thread function which takes a threadStruct as a parameter (more on this data structure in structs&funcs.h)
                calculates the cycles time and millisecond time and then waits for the proper time frame using nanosleep function 
                (to learn why I used this function check the readme file!)  
*/
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
    //a control loop for the modulo property of nsec 
    while(current->currentTime >= 1000){
        secondCtr++;
        current->currentTime -= 1000;
    }
    time.tv_nsec =  current->currentTime*1000000;
    time.tv_sec = secondCtr;
    nanosleep(&time, NULL); 
    return NULL;
}

/* runTimeCalc() - This function is a swtich which calculates the runTime in 
                   miliseconds for a specific task and returns it
*/
int runtTimeCalc(tasks *currentTask, config cycles){
    int runTime; 
    //switch to calculate the runtime of each type of task
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

/* simulation() - The heavy lifting function right here. It takes in the process queue, config structure 
                  and the thread parameter structure. It then goes through the process queue, for each process going through
                  each task queue and doing the needed wait and report for both I/O and processing tasks.
*/
void simulation(processQueue* processQ, config cycles, threadStruct threadParam){
    //declare and intialize variables 
    int processCounter = 1; 
    int threadCatch; 
    pthread_t thread_id; 
    tasks currentTask; 
    struct timespec startTime;
    struct timespec currentTime; 
    clock_gettime( CLOCK_REALTIME, &startTime); 
    struct timespec time;
    time.tv_sec = 0; 
    PCB currentProcess;

    // print the initial simulator display to file 
    clock_gettime( CLOCK_REALTIME, &currentTime); 
    outputOp(cycles, "- Simulator program starting", timeCompare(currentTime, startTime), '~');

    // loop through each process in process queue 
    while(emptyProcess(processQ) != 0){
        //get first process and prepare set state from ready to running 
        currentProcess = deqProcess(processQ); 
        printerFunct(currentTime,  startTime,  "Ready","- OS: preparing process", currentProcess, processCounter, cycles);
        printerFunct(currentTime,  startTime,  "Running","- OS: starting process", currentProcess, processCounter, cycles);

       //loop through the processes' task queue until empty  
       while(emptyTask(&currentProcess.taskQ) != 0){

            // dequeue the current task and start it 
            currentTask = deqTask(&(currentProcess.taskQ));
            clock_gettime( CLOCK_REALTIME, &currentTime);
            outputProcess(cycles, "- Process", ": start", currentTask.taskSpec, currentTask.taskID , timeCompare(currentTime, startTime), processCounter);

            // if current task is an I/O operation then do the waiting and calculation in a thread   
            if(strcmp(currentTask.taskID, "input") == 0 || strcmp(currentTask.taskID, "output") == 0) {

                //thread data/thread creation 
                threadParam.currentTask = currentTask; 
                threadCatch = pthread_create(&thread_id, NULL, ioTimer, (void*) &threadParam); 

                //thread error catching                 
                if(threadCatch){
                    printf("Error in thread creation.\n");
                }
                else{
                    pthread_join(thread_id, NULL); 
                }
                //end thread 
                pthread_cancel(thread_id);
            }

            // if it is a processing task do calculation in the function 
            else if(strcmp(currentTask.taskID, "action") == 0){
                    time.tv_nsec = cycles.processorCT*currentTask.cycleNum *1000000;
                    nanosleep(&time, NULL); 
            }

            //output time change 
            clock_gettime( CLOCK_REALTIME, &currentTime);
            outputProcess(cycles, "- Process", ": end", currentTask.taskSpec, currentTask.taskID , timeCompare(currentTime, startTime), processCounter);
        }

        //terminate process and increment the process counter 
        printerFunct(currentTime,  startTime,  "Terminated","- OS: removing process", currentProcess, processCounter, cycles);
        processCounter++;

        // if the scheduling is shortest remaining time first, then reorder the queue
        if(strcmp(cycles.sched, "SRTF") == 0){          
            shortestRunTime(processQ, cycles);
        }       
    }

    //calculate time and end simulator 
    clock_gettime( CLOCK_REALTIME, &currentTime);
    outputOp(cycles, "- Simulator program ending", timeCompare(currentTime, startTime), '~');

}

/* printerFunct() - This function is just an abstraction away from having  these three lines repeated in my simulator function. 
                    It takes in my two time variables, the two strings to output, config structure and the process counter. 
                    It then calculates time difference and calls the outputOp() function. 
*/
void printerFunct(struct timespec currentTime, struct timespec startTime, char* str1, char* str2, PCB currentProcess, int processCounter, config cycles){
    // get time, set state, and output 
    clock_gettime( CLOCK_REALTIME, &currentTime);
    currentProcess.processState = str1; 
    outputOp(cycles, str2, timeCompare(currentTime, startTime), processCounter);
}

/* outputOp() - Due to the two distinct types of printing which the operating system simulator muyst do, 
                I've split my printing up. This function does printing for the operating system instructions 
                and doesnt't a process state output */
void outputOp ( config cycles, char* inputString, float time, int processCounter){
    FILE* outputFile;  
    char processCstr[15];
    sprintf(processCstr, "%d", processCounter);

    // my stupid control for the start and end cases *so it doesn't output 126*
    if(processCounter == 126){
        processCstr[0] = ' '; 
        processCstr[1] = ' '; 
        processCstr[2] = ' ';
    }
    // Check if only outputting to the monitor
    if (cycles.logMethod == 'M' ) {
        printf( "%f %s %s \n", time, inputString, processCstr);
    }

    else{
        // Open the logPath file in append mode so it doesn't overwrite the contents
        outputFile = fopen(cycles.filePath, "a");

        // error catch for wrong configure file 
        if (outputFile == NULL){
            printf("Error: config Log File Path has incorrect formating\n");
            return; 
        }
        // Check if only outputting to the file
        if (cycles.logMethod == 'F'){
            // Output file contents with a precision of 6
            fprintf(outputFile, "%f %s %s \n",  time, inputString, processCstr);

            // Close the file
            fclose(outputFile);
        }

        // Check if logging to both
        if (cycles.logMethod == 'B'){
            //monitor output 
            printf( "%f %s %s \n", time, inputString, processCstr);

            // Open the logPath file in append mode so it doesn't overwrite the contents
            outputFile = fopen(cycles.filePath, "a");

            // Output file contents with a precision of 6
            fprintf(outputFile, "%f %s %s \n", time, inputString, processCstr);

            // Close the file
            fclose(outputFile);
        }
    }

}

/* outputProcess() - the output process for the second class of print actions, the process printing  
                     thr function takes in both the needed strings for process and start or end 
                     what task it is printing, the run-time, and the process number and outputs 
                     to the correct location (monitor, file or both)                     *
*/
void outputProcess ( config cycles, char* firstString, char*secondString, char* taskSpec, char*taskID, float time, int processCounter){
    // declare the file output pointer 
    FILE* outputFile;

    // Check if only outputting to the monitor
    if (cycles.logMethod == 'M' ){
        printf( "%f %s %d %s %s %s \n", time, firstString, processCounter, secondString, taskSpec, taskID);
    }

    else{
        // Open the logPath file in append mode so it doesn't overwrite the contents
        outputFile = fopen(cycles.filePath, "a");

        //error catch for the wrong config file path 
        if (outputFile == NULL){
            printf("Error: config Log File Path has incorrect formating\n");
            return; 
        }
        // Check if only outputting to the file
        if (cycles.logMethod == 'F'){
            // Output file contents with a precision of 6
            fprintf(outputFile, "%f %s %d %s %s %s \n", time, firstString, processCounter, secondString, taskSpec, taskID);
            // Close the file
            fclose(outputFile);
        }

        // Check if logging to both
        if (cycles.logMethod == 'B'){
            //output to the monitor 
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

/*  shortestRunTime() - a re-ordering of the process queue for SRTF scheduling algo. 
                        It recalculates the current runTime and then if it doesn't equal
                        the current process ID then it removes the process and re-inserts
                        it into the queue which puts it in the shorest runTime order. 
*/
void shortestRunTime(processQueue *Q, config cycles){
    // variable decleration and init 
    int runTime = 0; 
    PCBnode *currentProcess = Q->front;
    taskNode *currentTask;
    PCB swapProcess;  

    //loop throug the process queue 
    while(currentProcess != NULL) {
        runTime = 0; 
        currentTask = currentProcess->process.taskQ.front; 

        // loop through the task queue calculating runtime 
        while(currentTask != NULL){ 
            runTime += runtTimeCalc(&currentTask->task, cycles);
            currentTask = currentTask->ptr; 
        }
        // if not equal to the current runtime remove the process 
        // and reinsert it as to put it in shortest runtime order 
        if(currentProcess->process.processID != runTime){
            swapProcess = removeProcess(Q, currentProcess); 
            enqProcess(Q, &swapProcess); 
        }
        currentProcess = currentProcess->ptr;
    }
} 
