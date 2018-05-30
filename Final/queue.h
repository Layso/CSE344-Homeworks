#ifndef QUEUE_H
#define QUEUE_H



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0



struct Client {
    
};

struct node {
    struct Client data;
    struct node *next;
};

typedef struct Queue {
    struct node *head;
} Queue;



void QueueInitialize(Queue **queue);
void QueueDestruct(Queue *queue);
void QueueOffer(Queue *queue, struct Client item);
struct Client QueuePoll(Queue *queue, int *status);
int QueueEmpty(Queue *queue);


#endif