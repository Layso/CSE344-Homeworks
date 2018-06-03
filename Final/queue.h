#ifndef QUEUE_H
#define QUEUE_H

#include "server.h"

/* Struct to hold queue nodes */
struct node {
    struct Work data;
    struct node *next;
};

/* Struct to hold and use Queue structure */
typedef struct Queue {
    struct node *head;
    int size;
} Queue;



void QueueInitialize(Queue **queue);
void QueueDestruct(Queue *queue);
void QueueOffer(Queue *queue, struct Work item);
struct Work QueuePoll(Queue *queue);
int QueueEmpty(Queue *queue);
int QueueIsFull(Queue *queue);

#endif