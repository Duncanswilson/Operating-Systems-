/*
 * C Program to Implement Queue Data Structure using Linked List
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

////////////////////// DATA STRUCTURES /////////////////////////////////////////////////
/*meta-data(task)structure: 
  this data structure holds all of the salient information for a given task in the meta-data file
  It is used as the data in the dynamic queue defined below */
typedef struct{
// char task identifier
    char* taskID; 
// task specifier   
    char* taskSpec;
// int number of cycles
    int cycleNum; 
}tasks; 

/* Config structure:
   bundels all of the important information from the config file */
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
    it was created purely because the p_thread_create function will only allow one parameter */ 
typedef struct{
    tasks currentTask; 
    config cycles; 
    float currentTime;
}threadStruct;


/* node structure: 
    the data stored in the queue, used to bundle the pointer and task data */
typedef struct taskNode{
    tasks task;
    struct taskNode *ptr;
}taskNode; 

/* Queue structure: 
   the data structure used for storing the tasks from the meta-data file */
typedef struct{
  taskNode *front; 
  taskNode *rear; 
}taskQueue;

/*PCB structure: 
   Used to store the current state of the process, will
   be expanded greatly as we continue the simulation  */
typedef struct{
    char *processState; 
    //you could possibly include a Program Counter [if needed it will be added later]

    // the process number (either calculated off ofthe entry time or calculated remaining process time)
    int processID; 

    // the queue of the processes' meta-data
    taskQueue taskQ; 
    
}PCB;

/* node structure: 
    the data stored in the queue, used to bundle the pointer and process' data */
typedef struct PCBnode {
    PCB process;
    struct PCBnode *ptr;
}PCBnode; 

/* Queue structure: 
   the data structure used for storing the processes from the meta-data file */
typedef struct{
  PCBnode *front; 
}processQueue;


////////////////////// Suppoting Functions for Data Structures /////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void createProcess(processQueue*);
void enqProcess(processQueue*, PCB*);
PCB deqProcess(processQueue*);
int emptyProcess(processQueue*);
PCB removeProcess(processQueue *Q, PCBnode* target);
void displayP(processQueue* Q);


///////////////////////////////////////////////
void createTask(taskQueue*);
void enqTask(taskQueue*, tasks*);
tasks deqTask(taskQueue*);
int emptyTask(taskQueue*);
void displayT(taskQueue* Q);


/* create()- takes a queue as a parmater and "creates" an empty queue */
void createProcess(processQueue *Q)
{
    Q->front = NULL;
}
 
/*enq() -  takes the queue and task data as paramters then adds the task at the rear of the queue */
void enqProcess(processQueue *Q, PCB *data)
{
    PCBnode *temp = (PCBnode*)malloc(1*sizeof(PCBnode));
    temp->process = *data;
    PCBnode *previous = NULL;
    PCBnode *current = Q->front; 

	while(current != NULL && temp->process.processID >= current->process.processID){
        previous = current;
        current = current->ptr;
    }

    if(previous == NULL && current == NULL){
    	temp->ptr = NULL; 
        Q->front = temp; 
    }
    else if(previous == NULL){ 
	    temp->ptr = Q->front;
	    Q->front = temp; 
    }
    else{
        previous->ptr = temp;
        temp->ptr = current;
	}
}
 
/*deq()-  removes and returns the first element from the queue */
PCB deqProcess(processQueue* Q)
{
    PCBnode *front1 = Q->front;
    PCB  returnProcess; 
    if (front1 == NULL)
    {
        printf("\n Error: Trying to dequeue elements from empty queue");
        return returnProcess;
    }
    else
        if (front1->ptr != NULL)
        {
            returnProcess = Q->front->process; 
            front1 = front1->ptr;
            free(Q->front);
            Q->front = front1;
            return returnProcess;
        }
        else
        {
            returnProcess = Q->front->process; 
            free(Q->front);
            Q->front = NULL;
            return returnProcess;
        }
}
 

/* empty()-  shows if queue is empty or not */
int emptyProcess(processQueue* Q)
{
     if (Q->front == NULL)
        return 0; 
    else
       return 1; 
}

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


 //display() -  takes in the queue as a parameter and then displays the queue elements 
void displayP(processQueue* Q)
{
   PCBnode* current = Q->front;

    if(current == NULL)
    {
        printf("Process Queue is empty\n");
        return;
    } 
     printf("\nProcess ID:  %i \n", current->process.processID);
     printf("Task Queue: \n");
     displayT(&current->process.taskQ);
    while (current->ptr != NULL)
    {
        current = current->ptr;
        printf("\nProcess ID:  %i \n", current->process.processID);
        printf("Task Queue: \n");
        displayT(&current->process.taskQ);

    }
}



/////////////////////////////////

/* create()- takes a queue as a parmater and "creates" an empty queue */
void createTask(taskQueue *Q)
{
    Q->front = Q->rear = NULL;
}
 
/*enq() -  takes the queue and task data as paramters then adds the task at the rear of the queue */
void enqTask(taskQueue *Q, tasks *data)
{
    taskNode *temp; 
    if (Q->rear == NULL)
    {
    	//printf("Enqueue data: %s, %s, %i \n", data->taskID, data->taskSpec, data->cycleNum);
        Q->rear = (taskNode*)malloc(1*sizeof(taskNode));
        Q->rear->ptr = NULL;
        Q->rear->task = *data;
        Q->front = Q->rear;
    }
    else
    {
    	//printf("Enqueue data: %s, %s, %i \n", data->taskID, data->taskSpec, data->cycleNum);
        temp = (taskNode*)malloc(1*sizeof(taskNode));
        Q->rear->ptr = temp;
        temp->task = *data;
        temp->ptr = NULL;
        Q->rear = temp;
    }
}
 
/*deq()-  removes and returns the first element from the queue */
tasks deqTask(taskQueue* Q)
{
    taskNode *front1 = Q->front;
    tasks returnTask; 
    if (front1 == NULL)
    {
        printf("\n Error: Trying to dequeue elements from empty queue");
        return returnTask; 
    }
    else
        if (front1->ptr != NULL)
        {
            returnTask = Q->front->task; 
            front1 = front1->ptr;
            free(Q->front);
            Q->front = front1;
            return returnTask;
        }
        else
        {
            returnTask = Q->front->task; 
            free(Q->front);
            Q->front = NULL;
            Q->rear = NULL;
            return returnTask;
        }
}
 

/* empty()-  shows if queue is empty or not */
int emptyTask(taskQueue* Q)
{
     if ((Q->front == NULL) && (Q->rear == NULL))
        return 0; 
    else
       return 1; 
}

 //display() -  takes in the queue as a parameter and then displays the queue elements 
void displayT(taskQueue* Q)
{
   taskNode* front1 = Q->front;

    if ((front1 == NULL) && (Q->rear == NULL))
    {
        printf("Task Queue is empty\n");
        return;
    }     
    while (front1 != Q->rear)
    {
        printf("%s, %s, %i \n", front1->task.taskID, front1->task.taskSpec, front1->task.cycleNum);
        front1 = front1->ptr;

    }
    if (front1 == Q->rear)
      printf("%s, %s, %i \n", front1->task.taskID, front1->task.taskSpec, front1->task.cycleNum);
}
