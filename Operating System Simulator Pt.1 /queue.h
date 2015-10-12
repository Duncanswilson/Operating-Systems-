/*
 * C Program to Implement Queue Data Structure using Linked List
 */
#include <stdio.h>
#include <stdlib.h>

 
//meta-data structure 
typedef struct{
// char task identifier
    char* taskID; 
// task specifier   
    char* taskSpec;
// int number of cycles
    int cycleNum; 
}tasks; 

// data strucure for config file 
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
    char filePath[100];  
}config; 
 
typedef struct{
    tasks currentTask; 
    config cycles; 
    float currentTime; 
}threadStruct;

typedef struct node{
    tasks *info;
    struct node *ptr;
}node; 

typedef struct{
  node *front; 
  node *rear; 
}Queue;


tasks* frontelement();
void enq(Queue*, tasks*);
tasks* deq(Queue*);
int empty(Queue*);
void display(Queue*);
void create(Queue*);
void queuesize(Queue);
 
 
/* Create an empty queue */
void create(Queue *Q)
{
    Q->front = Q->rear = NULL;
}
 
/*void queuesize()
{
    printf("\n Queue size : %d", count);
}*/
 
/* Enqueing the queue */
void enq(Queue *Q, tasks *data)
{
    node *temp; 
    if (Q->rear == NULL)
    {
        Q->rear = (node*)malloc(1*sizeof(node));
        Q->rear->ptr = NULL;
        Q->rear->info = data;
 //       printf("%c, %c, %i \n", Q->rear->info->taskID, Q->rear->info->taskSpec, Q->rear->info->cycleNum);
 //       printf("%p \n", Q->rear);

        Q->front = Q->rear;
    }
    else
    {
        temp = (node*)malloc(1*sizeof(node));
        Q->rear->ptr = temp;
//      	printf("%p \n", temp);
        temp->info = data;
//        printf("%c, %c, %i \n", temp->info->taskID, temp->info->taskSpec, temp->info->cycleNum);

        temp->ptr = NULL;
        Q->rear = temp;
    }
}
 
/* Dequeing the queue */
tasks* deq(Queue* Q)
{
    node *front1 = Q->front;
    tasks * returnTask; 
    if (front1 == NULL)
    {
        printf("\n Error: Trying to dequeue elements from empty queue");
        return;
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
 
/* Returns the front element of queue */
tasks* frontelement(Queue* Q)
{
    if ((Q->front == NULL) && (Q->rear == NULL)){
        printf("%s\n", "Don't ask for the front element of empty queue, I'll be nice andr return null to protect you. :) ");
        return NULL;
     }   
    else
    return(Q->front->info);

 }
/* Display if queue is empty or not */
int empty(Queue* Q)
{
     if ((Q->front == NULL) && (Q->rear == NULL))
        return 0; 
    else
       return 1; 
}

/* Displaying the queue elements */
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

int fpeek(FILE *stream)
{
    int c;

    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}
/* 
void main()
{
    int no, ch, e;
 
    printf("\n 1 - Enque");
    printf("\n 2 - Deque");
    printf("\n 3 - Front element");
    printf("\n 4 - Empty");
    printf("\n 5 - Exit");
    printf("\n 6 - Display");
    printf("\n 7 - Queue size");
    create();
    while (1)
    {
        printf("\n Enter choice : ");
        scanf("%d", &ch);
        switch (ch)
        {
        case 1:
            printf("Enter data : ");
            scanf("%d", &no);
            enq(no);
            break;
        case 2:
            deq();
            break;
        case 3:
            e = frontelement();
            if (e != 0)
                printf("Front element : %d", e);
            else
                printf("\n No front element in Queue as queue is empty");
            break;
        case 4:
            empty();
            break;
        case 5:
            exit(0);
        case 6:
            display();
            break;
        case 7:
            queuesize();
            break;
        default:
            printf("Wrong choice, Please enter correct choice  ");
            break;
        }
    }
}*/
