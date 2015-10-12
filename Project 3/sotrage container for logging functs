
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


    //clear the queue of the initial tasks which don't correlate with printed data 
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
void logToFile(FILE* filePointer, threadStruct threadParam, config cycles, processQueue *processQ){
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

    //clear the queue of the initial tasks which don't correlate with printed data 
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

void logToBoth(FILE * filePointer, threadStruct threadParam, config cycles, processQueue *processQ){
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

    //clear the queue of the initial tasks which don't correlate with printed data 
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