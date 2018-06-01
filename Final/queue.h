#ifndef QUEUE_H
#define QUEUE_H

#include "server.h"



/* Struct to hold queue nodes */
struct node {
    struct Client data;
    struct node *next;
};

/* Struct to hold and use Queue structure */
typedef struct Queue {
    struct node *head;
} Queue;



void QueueInitialize(Queue **queue);
void QueueDestruct(Queue *queue);
void QueueOffer(Queue *queue, struct Client item);
struct Client QueuePoll(Queue *queue, int *status);
int QueueEmpty(Queue *queue);

#endif