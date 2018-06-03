#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



#define TRUE 1
#define FALSE 0
#define ZERO 0
#define PROVIDER_LIMIT 2


void QueueInitialize(Queue **queue) {
	if (*queue == NULL) {
		*queue = malloc(sizeof(Queue));
		(*queue)->head = NULL;
		(*queue)->size = ZERO;
	}
}



void QueueDestruct(Queue *queue) {
	struct node *current = NULL;
	struct node *destroy = NULL;
	
	

	if (queue != NULL) {		
		current = queue->head;
		while (current != NULL) {
			destroy = current;
			current = current->next;
			free(destroy);
		}

		free(queue);
	}
}



void QueueOffer(Queue *queue, struct Work item) {
	struct node *current = NULL;
	
	
	if (queue != NULL) {
		if (queue->head == NULL) {
			queue->head = malloc(sizeof(struct node));
			queue->head->next = NULL;
			queue->head->data = item;
		}
		
		else {
			current = queue->head;
			while (current->next != NULL) {
				current = current->next;
			}
			
			current->next = malloc(sizeof(struct node));
			current = current->next;
			current->next = NULL;
			current->data = item;
		}
		
		++(queue->size);
	}
}



struct Work QueuePoll(Queue *queue) {
	struct Work item;
	struct node *destroy = NULL;
	
	if (queue->head != NULL) {
		destroy = queue->head;
		item = destroy->data;
		queue->head = destroy->next;
		free(destroy);
		--(queue->size);
	}
	
	return item;
}



int QueueEmpty(Queue *queue) {
	return (queue == NULL || queue->head == NULL);
}


int QueueIsFull(Queue *queue) {
	return queue->size == PROVIDER_LIMIT;
}