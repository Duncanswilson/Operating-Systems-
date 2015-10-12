#include "structs&funcs.h"
#include "file_funcs.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
 
          ////// PLEASE READ THE READ ME FILE FOR CLARIFICATION ON WHY EVERYTHING IS THE WAY IT IS    ///////
             ///// I would  mark myself down if I got this code without any preface or methodology ////////

int runtTimeCalc(tasks *currentTask, config cycles); 
float timeCompare(struct timespec currentTime, struct timespec startTime);
void simulation(processQueue* processQ, config cycles);
void waitFunction(config cycles, int *interrupt, processQueue *readyQ, processQueue *blockedQ, struct timespec startTime);
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
    int rrFlag = 0; 
    int charConvert;
    int cycleFlag; 
    int pid = 1; 

    // a task buffer to hold the data from the meta-Data
    tasks *testTask = (tasks*)malloc(1*sizeof(tasks));

    // pcb is a data structure for storing important process data (specifically the task Queue)
    PCB* currentProcess = (PCB*)malloc(1*sizeof(PCB));

    // set up queue of processes
    processQueue *processQ = (processQueue*)malloc(1*sizeof(processQueue));
    createProcess(processQ);


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
     if(strcmp(cycles.sched,"RR") == 0){
        rrFlag = 1; 
     }

     //get quantum time 
     fgets(buffer, 24, filePointer); 
     fgetstr(buffer, 100, filePointer);
     cycles.quantum = atoi(buffer); 
     if(cycles.quantum == 0){
        printf("Time Quantum of zero entered, ending simulator. \n");
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
        taskStack* taskS = (taskStack*)malloc(1*sizeof(taskStack));
        createTask(taskS);

        //prime the task loading loop 
        cycleFlag = taskParse (testTask, &filePointer);

        // if the prime value is -1 then buffer out the newline 
         if(cycleFlag == -1){ 
         fgetc(filePointer); 
         //reprime the loop 
         cycleFlag = taskParse (testTask, &filePointer);
         }

        //loop until end of process (cycleFlag = 2 or 3 ) or end of file ( cycleFlag = 0 )
        while(cycleFlag != 2 && cycleFlag != 0 && cycleFlag != 3){
             //load the current task into the task queue
             enqTask(taskS, testTask);

             // if cycleFlag =  -1 then buffer out the newline 
             if(cycleFlag == -1){ 
             fgetc(filePointer); 
             }

             //get the next task 
             cycleFlag = taskParse (testTask, &filePointer);
        }

        //if not round robin  
        if(rrFlag == 0 ){
            // set current process state to ready and set processID and priority 
            currentProcess->processState = "ready";
            currentProcess->processID = pid; 
            currentProcess->priority = pid; 
        }
        else{
            // set current process state to ready and set processID and priority 
            currentProcess->processState = "ready";
            currentProcess->processID = pid; 
            currentProcess->priority = 0; 
        }

        // if it is the data is "load-worthy", then enqueue it 
        if(cycleFlag != 0 && cycleFlag != 3 ){
            currentProcess->taskS = *taskS;
            enqProcess(processQ, currentProcess);
            //index process id
            pid++; 
         }    

        // deallocate the taskS memory  
        free(taskS);

        //reset flag for next process 
        if(cycleFlag == 2)
            cycleFlag = 1;
    }

    //close the  file connection 
   fclose(filePointer); 


///////////////////////////////// Start Simulation ////////////////////////////////////////////////////////////////////

    simulation(processQ, cycles);
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
                After it has simulated the running for a particular amount of time, it then checks if there is already a process
                returning and waits until the interrupt buffer is clear. Then the function (called by a thread) sets the pid in the 
                interrupt buffer and exits   
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

    // interrupt buffer check and wait 
    while(*(current->interruptPtr) != 0){}

    //set the interrupt buffer with the interrupt processes' ID and exit 
    *(current->interruptPtr) = current->currentProcess.processID;
    free(current); 
    pthread_exit(NULL);
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
                  each task queue and doing the needed wait and report for both I/O and processing tasks. To incorporate 
                  multi-threading this function has grown substatially and I'll keep the genral methodology in the README 
                  file, but I'l also added some section comments to make reading the code easier 
*/
void simulation(processQueue* readyQ, config cycles){
    //declare and intialize variables 
    int threadCatch; 
    int cycleIter; 
    int interrupt = 0; 
    int completeFlag = 0; 
    pthread_t thread_id; 
    tasks currentTask; 
    tasks interruptedTask;
    struct timespec startTime;
    struct timespec currentTime; 
    clock_gettime( CLOCK_REALTIME, &startTime); 
    struct timespec time;
    time.tv_sec = 0; 
    PCB currentProcess;
    PCB blockedProcess; 
    processQueue *blockedQ = (processQueue*)malloc(1*sizeof(processQueue));
    createProcess(blockedQ);

/* Simulator Start  */
    clock_gettime(CLOCK_REALTIME, &currentTime); 
    outputOp(cycles, "- Simulator program starting", timeCompare(currentTime, startTime), '~');

/* Main Loop: the simulator loops until both the ready and blocked queues are empty  */  
    while(emptyProcess(readyQ) != 0 || emptyProcess(blockedQ) != 0){

    /* check if the blocked queue is currently the only queue with items */
        if(emptyProcess(readyQ) == 0 && emptyProcess(blockedQ) != 0){ 
                // if so go to the subroutine for waiting on a process to return 
                waitFunction(cycles, &interrupt, readyQ, blockedQ, startTime);
            }
    /*if not then the ready queue has items in it */ 
        else{
             //get first process and prepare set state from ready to running 
            currentProcess = deqProcess(readyQ); 
            printerFunct(currentTime,  startTime,  "Ready","- OS: selecting next process", currentProcess, '~', cycles);
            printerFunct(currentTime,  startTime,  "Running","- OS: starting process", currentProcess, currentProcess.processID, cycles);

            // pop the current task and start it 
            currentTask = popTask(&(currentProcess.taskS));
            clock_gettime(CLOCK_REALTIME, &currentTime);
            outputProcess(cycles, "- Process", ": start", currentTask.taskSpec, currentTask.taskID , timeCompare(currentTime, startTime), currentProcess.processID);

        /* I/O Task Section: 
            For each I/O task load all of the salient data into the threadStruct and then create a new thread
            I push the "just dispached" task back onto the task stack as the simulator needs it for printing upon return 
            Then the process is enqueued to the blocked queue 
        */ 
            if(strcmp(currentTask.taskID, "input") == 0 || strcmp(currentTask.taskID, "output") == 0) {
                //create the individual thread parameter,(if not dynamic then overlap occurs)
                // the memory is freed in the thread functions themselves 
                threadStruct *threadParam = (threadStruct*)malloc(1*sizeof(threadStruct));
                threadParam->cycles = cycles;
                threadParam->interruptPtr = &interrupt;
                threadParam->currentProcess = currentProcess;
                threadParam->currentTask = currentTask;
                // push the thread task back onto the task stack for printing data!
                pushTask(&(currentProcess.taskS), &currentTask);
                threadCatch = pthread_create(&thread_id, NULL, ioTimer, (void*) threadParam); 
                //thread error catching                 
                if(threadCatch){
                    printf("Error in thread creation.\n");
                }
                printerFunct(currentTime,  startTime,  "Blocked","- OS: Blocked Queue Loaded with Process", currentProcess, currentProcess.processID, cycles);
                enqProcess(blockedQ, &currentProcess); 
            }

        /* Processing Task Section: 
            Loop through running one cycle of processor action until there is: 
             - an interrupt: 
                - In which case you remove the returning process from the blocked queue 
                - Output returning process info and enqueue back into ready 
             - the time quantum is hit: 
                - in which case you exit the loop normally and push the unfinished 
                  task back onto the stack
             - the processing task completes: 
                - print results and possibly add it back to ready 
        */
            else if(strcmp(currentTask.taskID, "action") == 0){
                for(cycleIter = 0; cycleIter < cycles.quantum && completeFlag != 1; cycleIter++){
                    // if there isn't an interupt run one cycle for the current process through the processor  
                    // decrementing the number of cycles left in the current task and checking for completion 
                    if(interrupt == 0){
                        time.tv_nsec = cycles.processorCT*1000000;
                        nanosleep(&time, NULL); 
                        currentTask.cycleNum--;
                        if(currentTask.cycleNum == 0){
                            completeFlag = 1; 
                        } 
                    }
                    // if there is an interrupt 
                    else{
                         //remove the blocked process from the blocked queue via the PID saved in interrupt 
                        blockedProcess = removeProcess(blockedQ, interrupt); 
                        interruptedTask = popTask(&(blockedProcess.taskS));
                        clock_gettime( CLOCK_REALTIME, &currentTime);
                        outputProcess(cycles, "- Process", ": end", interruptedTask.taskSpec, interruptedTask.taskID , timeCompare(currentTime, startTime), blockedProcess.processID);
                        printerFunct(currentTime,  startTime,  "Ready","- OS: Ready Queue Loaded with Process", blockedProcess, blockedProcess.processID, cycles);
                        enqProcess(readyQ, &blockedProcess); 
                        // end the loop 
                        cycleIter = cycles.quantum; 
                        //reset interrupt 
                        interrupt = 0;
                    }
                }    
               // if the processing task was completed 
               if (completeFlag == 1){
                  //output time change 
                  clock_gettime( CLOCK_REALTIME, &currentTime);
                  outputProcess(cycles, "- Process", ": end", currentTask.taskSpec, currentTask.taskID , timeCompare(currentTime, startTime), currentProcess.processID);
                }

               // if the time quantum interrupted the task 
               else {
                  clock_gettime( CLOCK_REALTIME, &currentTime);
                  printerFunct(currentTime,  startTime,  "Ready","- OS: quantum time interrupt process ", currentProcess, currentProcess.processID, cycles);
                  //push the unfinshed task back into the task stack 
                  pushTask(&(currentProcess.taskS), &currentTask); 
                }

                //check if the process still has tasks to do and if so then enqueue it back to ready 
                if(emptyTask(&(currentProcess.taskS)) != 0 ){
                    //enqueue the unfished current process 
                    enqProcess(readyQ, &currentProcess);
                    printerFunct(currentTime,  startTime,  "Ready","- OS: Ready Queue Loaded with Process", currentProcess, currentProcess.processID, cycles);

                } 
            }

        /* Interrupt Checker: 
            If an interrupt comes back mid-iteration of the "OS" then the simulator handles it here  
        */ 
            if(interrupt != 0){
                 //remove the blocked process from the blocked queue via the PID saved in interrupt 
                blockedProcess = removeProcess(blockedQ, interrupt); 
                interruptedTask = popTask(&(blockedProcess.taskS));
                clock_gettime( CLOCK_REALTIME, &currentTime);
                outputProcess(cycles, "- Process", ": end", interruptedTask.taskSpec, interruptedTask.taskID , timeCompare(currentTime, startTime), blockedProcess.processID);
                //enqueue interrupted processe to the ready queue 
                enqProcess(readyQ, &blockedProcess); 
                //reset interrupt 
                interrupt = 0; 
            }
        /* Process Terminator:
            if the current process has nothing left in it's task stack then set it to terminated
            and print that it has been removed  
        
        */ 
           if(emptyTask(&(currentProcess.taskS)) == 0){
               //terminate process and increment the process counter 
               printerFunct(currentTime,  startTime,  "Terminated","- OS: removing process", currentProcess, currentProcess.processID, cycles);
            }

        // if the scheduling is shortest remaining time first, then reorder the queue
        if(strcmp(cycles.sched, "SRTF") == 0 ||strcmp(cycles.sched, "SRTF-P") == 0){          
            shortestRunTime(readyQ, cycles);
        }       
      }
    }

/* Simulator Conclusion: calculates completion time and ends simulator */
    clock_gettime( CLOCK_REALTIME, &currentTime);
    outputOp(cycles, "- Simulator program ending", timeCompare(currentTime, startTime), '~');
}

/* printerFunct() - This function is a subroutine for the simulator which is used for the case when the ready queue is empty 
                    It waits for the interrupt signal and then handles output. It will enqueue it to the ready queue 
                    if there are tasks still on the task stack. If the task stack is empty then it removes the process 
*/
void waitFunction(config cycles, int *interrupt, processQueue *readyQ, processQueue *blockedQ, struct timespec startTime){
    PCB temp;
    tasks interruptedTask;
    struct timespec currentTime;
    //wait until the i/o task returns 
    while(*(interrupt) == 0 ){}
    temp = removeProcess(blockedQ, *interrupt); 
    //reset the interrupt
    *interrupt = 0;
    //output the completion of the returned i/o operation 
    interruptedTask = popTask(&(temp.taskS));
    clock_gettime( CLOCK_REALTIME, &currentTime);
    outputProcess(cycles, "- Process", ": end", interruptedTask.taskSpec, interruptedTask.taskID , timeCompare(currentTime, startTime), temp.processID);
    if(emptyTask(&(temp.taskS)) != 0){
        // put completed process back into the queue
        enqProcess(readyQ, &temp);
    }
    else{
        //terminate process and increment the process counter 
        printerFunct(currentTime,  startTime,  "Terminated","- OS: removing process", temp, temp.processID, cycles);
    }

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

/* outputOp() - Due to the two distinct types of printing which the operating system simulator must do, 
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
void outputProcess ( config cycles, char* firstString, char*secondString, char* taskSpec, char*taskID, float time, int processCounter ){
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

    //loop through the process queue 
    while(currentProcess != NULL) {
        runTime = 0; 
        currentTask = currentProcess->process.taskS.front; 

        // loop through the task queue calculating runtime 
        while(currentTask != NULL){ 
            runTime += runtTimeCalc(&currentTask->task, cycles);
            currentTask = currentTask->ptr; 
        }
        // if not equal to the current runtime remove the process 
        // and reinsert it as to put it in shortest runtime order 
        if(currentProcess->process.processID != runTime){
            swapProcess = removeProcess(Q, currentProcess->process.processID); 
            enqProcess(Q, &swapProcess); 
        }
        currentProcess = currentProcess->ptr;
    }
} 
