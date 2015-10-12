/*
 * C Program to Implement Queue Data Structure using Linked List
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

////////////////////// DATA STRUCTURES /////////////////////////////////////////////////
/*PCB structure: 
   Used to store the current state of the process, will
   be expanded greatly as we continue the simulation  */
typedef struct{
    char *processState; 
    //you could possibly include a Program Counter [if needed it will be added later]
    
}PCB;
 
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
    the data stored in the queue, used to bundel the pointer and task data */
typedef struct node{
    tasks *info;
    struct node *ptr;
}node; 

/* Queue structure: 
   the data structure used for storing the tasks from the meta-data file */
typedef struct{
  node *front; 
  node *rear; 
}Queue;


////////////////////// Suppoting Functions for Data Structures /////////////////////////////////////////////////

tasks* frontelement();
void enq(Queue*, tasks*);
tasks* deq(Queue*);
int empty(Queue*);
void display(Queue*);
void create(Queue*);
void queuesize(Queue);
 
 

/* create()- takes a queue as a parmater and "creates" an empty queue */
void create(Queue *Q)
{
    Q->front = Q->rear = NULL;
}
 

/*enq() -  takes the queue and task data as paramters then adds the task at the rear of the queue */
void enq(Queue *Q, tasks *data)
{
    node *temp; 
    if (Q->rear == NULL)
    {
        Q->rear = (node*)malloc(1*sizeof(node));
        Q->rear->ptr = NULL;
        Q->rear->info = data;
        Q->front = Q->rear;
    }
    else
    {
        temp = (node*)malloc(1*sizeof(node));
        Q->rear->ptr = temp;
        temp->info = data;
        temp->ptr = NULL;
        Q->rear = temp;
    }
}
 
/*deq()-  removes and returns the first element from the queue */
tasks* deq(Queue* Q)
{
    node *front1 = Q->front;
    tasks * returnTask; 
    if (front1 == NULL)
    {
        printf("\n Error: Trying to dequeue elements from empty queue");
        return NULL;
    }
    else
        if (front1->ptr != NULL)
        {
            returnTask = Q->front->info; 
            front1 = front1->ptr;
            free(Q->front);
            Q->front = front1;
            return returnTask;
        }
        else
        {
            returnTask = Q->front->info; 
            free(Q->front);
            Q->front = NULL;
            Q->rear = NULL;
            return returnTask;
        }
}
 

/* empty()-  shows if queue is empty or not */
int empty(Queue* Q)
{
     if ((Q->front == NULL) && (Q->rear == NULL))
        return 0; 
    else
       return 1; 
}

/* display() -  takes in the queue as a parameter and then displays the queue elements */
void display(Queue* Q)
{
   node* front1 = Q->front;

    if ((front1 == NULL) && (Q->rear == NULL))
    {
        printf("Queue is empty");
        return;
    }     
    while (front1 != Q->rear)
    {
        printf("%p \n", front1->ptr);
        printf("%s, %s, %i \n", front1->info->taskID, front1->info->taskSpec, front1->info->cycleNum);
        front1 = front1->ptr;

    }
    if (front1 == Q->rear)
      printf("%s, %s, %i \n", front1->info->taskID, front1->info->taskSpec, front1->info->cycleNum);
}

