#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



#define TRUE 1
#define FALSE 0



void QueueInitialize(Queue **queue) {
	if (*queue == NULL) {
		*queue = malloc(sizeof(Queue));
		(*queue)->head = NULL;
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



void QueueOffer(Queue *queue, struct Client item) {
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
	}
}



struct Client QueuePoll(Queue *queue, int *status) {
	struct Client item;
	struct node *destroy = NULL;
	*status = FALSE;
	
	if (queue->head != NULL) {
		*status = TRUE;
		destroy = queue->head;
		item = destroy->data;
		queue->head = destroy->next;
		free(destroy);
	}
	
	return item;
}



int QueueEmpty(Queue *queue) {
	return (queue == NULL || queue->head == NULL);
}