int getCycle(FILE *filePointer);
int taskParse(tasks *currentTask, FILE **filePointer);
int fpeek(FILE *stream);

/* getCyclce() - this reading in the config file which loads it into the config data structure*/
int getCycle(FILE *filePointer){
     int test; 
     char cycleBuffer[30];
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

/*taskParse() - an ugly and long function for seperating the metadata file and loading it into the tasks data structure
                it takes in a pointer to the current task a pointer to the file pointer in order to keep it in place 
                a lot of this function could be slimmed down and I'd love feedback on what to change in it */
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

    // this switch converts from the inital character into a printable string and stores it in the data structure 
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

    // this switch converts the inital character into the printable string and stores it in the data structure 
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
   
   // loop through the rest of the current tasks characters  
    buffer = fgetc(*filePointer); 
    while((char)buffer != ';' && (char)buffer != '.'){  
     cycleNum[iterator] = buffer; 
     iterator++; 
     buffer = fgetc(*filePointer); 
     }
     (*currentTask).cycleNum = atoi(cycleNum);
     
    //it uses a roll your own fpeek function to check if the next character is a newline
    //if so it returns a -1 to buffer out the newline char 
    //the else is for if the buffer is a period indicating the EOF 
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

//fgetstr() - mimics behavior of fgets(), but removes new-line
 //   character at end of line if it exists
 char *fgetstr(char *string, int n, FILE *stream)
 {
   char *result;
  result = fgets(string, n, stream);
   if(!result)
       return(result);
 
  if(string[strlen(string) - 1] == '\n')
      string[strlen(string) - 1] = 0;

  return(string);
}

/* fpeek()- a reproduction of fpeek from c++ in c */
int fpeek(FILE *stream)
{
    int c;

    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}
