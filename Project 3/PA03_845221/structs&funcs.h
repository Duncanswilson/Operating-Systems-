/*
 * C Program to Implement Queue Data Structure using Linked List
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

////////////////////// DATA STRUCTURES /////////////////////////////////////////////////
/*task structure: 
  wraps all of the salient information for a given task in the meta-data file
  It is used as the data in the dynamic queue defined below
*/
typedef struct{
// char task identifier
    char* taskID; 
// task specifier   
    char* taskSpec;
// int number of cycles
    int cycleNum; 
}tasks; 

/* Config structure:
   wraps all of the important information from the config file 
*/
typedef struct {
    //processor cycle time
    int processorCT; 
    //montior cycle time
    int monitorCT; 
    //hdd cycle time
    int hddCT; 
    //print cycle time
    int printCT;  
    //keyboard cycle time
    int keyboardCT; 
    //log method 
    char logMethod;
    //log file path 
    char filePath[75];  
    //meta-data path 
    char metaPath[75];
    //scheduling type 
    char sched[25];
}config; 
 
/* thread structure: 
    this data structure holds all of the information needed to be passed into the thread function 
    it was created purely because the p_thread_create function will only allow one parameter 
*/ 
typedef struct{
    tasks currentTask; 
    config cycles; 
    float currentTime;
}threadStruct;


/* task-node structure: 
    the data stored in the queue, used to bundle the pointer and task data */
typedef struct taskNode{
    tasks task;
    struct taskNode *ptr;
}taskNode; 

/* task Queue structure: 
   the data structure used for storing the tasks from the meta-data file */
typedef struct{
  taskNode *front; 
  taskNode *rear; 
}taskQueue;

/*PCB structure: 
   Used to store:   the current state of the process
                    the processID (aka the priority)
                    the queue of tasks the process is to run
*/
typedef struct{
    char *processState; 
    int processID; 
    taskQueue taskQ; 
}PCB;

/* PCB node structure: 
    the PCB data stored in the process queue, used to bundle the pointer and process data 
*/
typedef struct PCBnode {
    PCB process;
    struct PCBnode *ptr;
}PCBnode; 

/* Process Queue structure: 
    the data structure used for storing the processes from the meta-data file 
    this is a priority queue that orders the PCB's from least to greatest (on their processID)
    If two processIDs equal then put the newer entry behind the matching process
    A worthwhile note: *I removed the rear pointer when dealing with priority,
                        it just caused more headache since I can use the NULL
                        to tell if I'm at the end of queue *  
*/
typedef struct{
  PCBnode *front; 
}processQueue;


////////////////////// Suppoting Functions for Data Structures /////////////////////////////////////////////////
//
/////////////////////////////////// Processs Queue Functions Headers////////////////////////////////////////////
void createProcess(processQueue*);
void enqProcess(processQueue*, PCB*);
PCB deqProcess(processQueue*);
int emptyProcess(processQueue*);
PCB removeProcess(processQueue *Q, PCBnode* target);
void displayP(processQueue* Q);
//
/////////////////////////////////////// Task Queue Functions Headers//////////////////////////////////////////////
void createTask(taskQueue*);
void enqTask(taskQueue*, tasks*);
tasks deqTask(taskQueue*);
int emptyTask(taskQueue*);
void displayT(taskQueue* Q);
//
/////////////////////////////////// Processs Queue Functions Implementation///////////////////////////////////////
//
/* create()- takes a queue as a parmater and "creates" an empty queue */
void createProcess(processQueue *Q){
    Q->front = NULL;
}
 
/*enq() -  it inserts the PCB data into the process queue in order by processID 
            this creates a priortiy queue where if two processIDs equal 
            the newest PCB is stored behind the already existing entry  
*/
void enqProcess(processQueue *Q, PCB *data){
    //variable allocation and decleration 
    PCBnode *temp = (PCBnode*)malloc(1*sizeof(PCBnode));
    temp->process = *data;
    PCBnode *previous = NULL;
    PCBnode *current = Q->front; 

    //get to the right place to inset the process into queue off of priority 
    while(current != NULL && temp->process.processID >= current->process.processID){
        previous = current;
        current = current->ptr;
    }
    // if empty then load it 
    if(previous == NULL && current == NULL){
        temp->ptr = NULL; 
        Q->front = temp; 
    }
    // if highest priority then load it
    else if(previous == NULL){ 
        temp->ptr = Q->front;
        Q->front = temp; 
    }
    //if loading in the middle or at the back 
    else{
        previous->ptr = temp;
        temp->ptr = current;
    }
}
 
/*deq()-  removes and returns the first element from the queue */
PCB deqProcess(processQueue* Q){
    PCBnode *front1 = Q->front;
    PCB  returnProcess; 
    // if empty do nothing 
    if (front1 == NULL){
        printf("\n Error: Trying to dequeue elements from empty queue");
        return returnProcess;
    }
   //if not empty, remove first item 
    else
        if (front1->ptr != NULL){
            returnProcess = Q->front->process; 
            front1 = front1->ptr;
            free(Q->front);
            Q->front = front1;
            return returnProcess;
        }
        else{
            returnProcess = Q->front->process; 
            free(Q->front);
            Q->front = NULL;
            return returnProcess;
        }
}
 

/* empty()-  shows if queue is empty or not */
int emptyProcess(processQueue* Q){
     if (Q->front == NULL)
        return 0; 
    else
       return 1; 
}

/*remove() -  removes a process from the process with the address of target
              and then returns that PCB 
*/
PCB removeProcess(processQueue *Q, PCBnode* target){
    PCBnode *previous = NULL; 
    PCBnode *current = Q->front;
    PCB returnProcess; 

    while(&(*previous->ptr) != target ){
        previous = current; 
        current = current->ptr; 
    }
    returnProcess = current->process;
    previous->ptr = current->ptr; 
    free(current); 
    return returnProcess;
}


/*display() -  takes in the queue as a parameter and then displays the queue elements */
void displayP(processQueue* Q){
   PCBnode* current = Q->front;

    if(current == NULL){
        printf("Process Queue is empty\n");
        return;
    } 
     printf("\nProcess ID:  %i \n", current->process.processID);
     printf("Task Queue: \n");
     displayT(&current->process.taskQ);
    while (current->ptr != NULL){
        current = current->ptr;
        printf("\nProcess ID:  %i \n", current->process.processID);
        printf("Task Queue: \n");
        displayT(&current->process.taskQ);
    }
}

/////////////////////////////////// Task Queue Functions Implementation///////////////////////////////////////

/* create()- takes a queue as a parmater and "creates" an empty queue */
void createTask(taskQueue *Q){
    Q->front = Q->rear = NULL;
}
 
/*enq() -  takes the queue and task data as paramters then adds the task at the rear of the queue */
void enqTask(taskQueue *Q, tasks *data){
    taskNode *temp; 
    if (Q->rear == NULL){
        Q->rear = (taskNode*)malloc(1*sizeof(taskNode));
        Q->rear->ptr = NULL;
        Q->rear->task = *data;
        Q->front = Q->rear;
    }
    else{
        temp = (taskNode*)malloc(1*sizeof(taskNode));
        Q->rear->ptr = temp;
        temp->task = *data;
        temp->ptr = NULL;
        Q->rear = temp;
    }
}
 
/*deq()-  removes and returns the first element from the queue */
tasks deqTask(taskQueue* Q){
    taskNode *front1 = Q->front;
    tasks returnTask; 
    if (front1 == NULL){
        printf("\n Error: Trying to dequeue elements from empty queue");
        return returnTask; 
    }
    else{
        if (front1->ptr != NULL){
            returnTask = Q->front->task; 
            front1 = front1->ptr;
            free(Q->front);
            Q->front = front1;
            return returnTask;
        }
        else{
            returnTask = Q->front->task; 
            free(Q->front);
            Q->front = NULL;
            Q->rear = NULL;
            return returnTask;
        }
    }
}

/* empty()-  shows if queue is empty or not */
int emptyTask(taskQueue* Q){
     if ((Q->front == NULL) && (Q->rear == NULL)){
        return 0; 
    }
    else{
       return 1; 
    }
}

 /*display() -  takes in the queue as a parameter and then displays the queue elements */
void displayT(taskQueue* Q){
   //iterator variable assignment 
   taskNode* front1 = Q->front;

    if ((front1 == NULL) && (Q->rear == NULL)){
        printf("Task Queue is empty\n");
        return;
    }     
    while (front1 != Q->rear){
        printf("%s, %s, %i \n", front1->task.taskID, front1->task.taskSpec, front1->task.cycleNum);
        front1 = front1->ptr;
    }
    if (front1 == Q->rear){
      printf("%s, %s, %i \n", front1->task.taskID, front1->task.taskSpec, front1->task.cycleNum);
    }
}
