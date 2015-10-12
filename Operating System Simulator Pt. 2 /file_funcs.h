int getCycle(FILE *filePointer);
int taskParse(tasks *currentTask, FILE **filePointer);
int fpeek(FILE *stream);

/* getCyclce() - this read-in function parses the config file
                 and then return the cycle number to be loaded 
                 into the config struct
*/
int getCycle(FILE *filePointer){
    //variable declerations 
     int test; 
     char cycleBuffer[30];
     test = fgetc(filePointer);
     
     // loop until stop character 
     while ((char)test != ':'){
     test = fgetc(filePointer);
     }
    //get space out 
     int fileInc = 0;
     test = fgetc(filePointer);

     //loop until end of line 
     while((char)test != '\n'){
     test = fgetc(filePointer);
     cycleBuffer[fileInc] = (char)test; 
     fileInc++;
     }
     test =  atoi(cycleBuffer);
    return atoi(cycleBuffer);
}

/*taskParse() - an ugly and long function for seperating the metadata file and loading it into the tasks data structure
                it takes in a pointer to the current task a pointer to the file pointer in order to keep it in place 
                a lot of this function could be slimmed down and I'd love feedback on what to change in it 
                it uses a roll your own fpeek function to check if the next character is a newline
            TaskParse returns an integer with 5 meaningful values: 
            -1 - get the newline 
             0 - end of file (denoted by a period in meta-data)
             1 - normal task read-in 
             2 - end of current process flag (don't add to queue)
             3 - end of system simulator (don't add to queue)

If you have any improvements to this function please write them in the grading commments!! -Thanks :) 
*/
int taskParse(tasks *currentTask, FILE **filePointer){
    char buffer; 
    char cycleNum[5];
    int iterator = 0;
    int endFlag = 0;
    int systemFlag = 0;  
    char taskID, taskSpec; 
    // set the task ID via a switch from char to string for printing ease 
    for(iterator = 0; iterator < 6; iterator++){
        cycleNum[iterator] = 'n'; 
    }
    iterator = 0;
    // this switch converts from the inital character into a printable string and stores it in the data structure 
    taskID = fgetc(*filePointer);
    switch(taskID){
        case 'S':
            (*currentTask).taskID = "system";
            systemFlag = 1;  
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

    // this switch converts the inital character into the printable string and stores it in the data structure 
    taskSpec = fgetc(*filePointer); 
    switch(taskSpec){
        case 's':
            (*currentTask).taskSpec = "start";
            systemFlag = endFlag = 1; 
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
            endFlag =1; 
            break;  
    }
    while((char)buffer != ')'){
     buffer = fgetc(*filePointer); 
    }
   
   // loop through the rest of the current tasks characters  
    buffer = fgetc(*filePointer); 
    while((char)buffer != ';' && (char)buffer != '.'){  
     cycleNum[iterator] = buffer; 
     iterator++; 
     buffer = fgetc(*filePointer); 
     }
     (*currentTask).cycleNum = atoi(cycleNum);
     
     // For return value key read the block comment at beginning of function 
    //Below I use a roll your own fpeek function to check if the next character is a newline
    //At the end of the task 
    if((char)buffer == ';') {
     fgetc(*filePointer);
        // if next character is newline then return -1
       if((char)fpeek(*filePointer) == '\n'){
          return -1; 
        }
       // endFlag means taskParse is at the end of a process or system  
       if(endFlag == 1){
          //systemFlag means that taskParse is at the S(end) task 
       	  if(systemFlag == 1){
       	  	return 3; 
       	  }
          //end of process 
          return 2; 
       }
        //normal read in 
        return 1; 
     }
    else{ 
        //end of file return 
        return 0; 
     }
 }

/*fgetstr() - mimics behavior of fgets(), but removes new-line
              character at end of line if it exists
*/
char *fgetstr(char *string, int n, FILE *stream){
   char *result;
  result = fgets(string, n, stream);
   if(!result)
       return(result);
 
  if(string[strlen(string) - 1] == '\n')
      string[strlen(string) - 1] = 0;

  return(string);
}

/* fpeek()- a reproduction of fpeek from c++ in c 
*/
int fpeek(FILE *stream){
    int c;
    c = fgetc(stream);
    ungetc(c, stream);
    return c;
}
