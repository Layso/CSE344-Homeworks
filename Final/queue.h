#ifndef QUEUE_H
#define QUEUE_H



/* Struct to hold queue nodes */
struct node {
    int data;
    struct node *next;
};

/* Struct to hold and use Queue structure */
typedef struct Queue {
    struct node *head;
    int size;
} Queue;



void QueueInitialize(Queue **queue);
void QueueDestruct(Queue *queue);
void QueueOffer(Queue *queue, int item);
int QueuePoll(Queue *queue);
int QueueEmpty(Queue *queue);
int QueueIsFull(Queue *queue);

#endif