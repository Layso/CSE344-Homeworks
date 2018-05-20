#include "151044001_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <time.h>


#include "queue.h"



Queue **workQueue = NULL;
pthread_cond_t *conditionVariables = NULL;
pthread_mutex_t *mutexes = NULL;
int keepWorking = TRUE;



int main(int argc, char **argv) {
	int i;
	int floristCount, clientCount;
	pthread_t *threadArray = NULL;
	struct Client *clients = NULL;
	struct Florist *florists = NULL;
	int a;
	
	if (argc != REQUIRED_ARGC) {
		fprintf(stderr, "Usage: %s [fileName]\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	
	ParseFile(argv[1], &florists, &clients, &floristCount, &clientCount);
	mutexes = malloc(sizeof(pthread_mutex_t) * floristCount);
	workQueue = malloc(sizeof(Queue*) * floristCount);
	threadArray = malloc(sizeof(pthread_t) * floristCount);
	conditionVariables = malloc(sizeof(pthread_cond_t) * floristCount);
	for (i=0; i<floristCount; ++i) {
		workQueue[i] = NULL;
		QueueInitialize(workQueue+i);
		pthread_mutex_init(mutexes+i, NULL);
		pthread_cond_init(conditionVariables+i, NULL);
		pthread_create(threadArray+i, NULL, FloristThread, florists+i);
	}
	
	for (i=0; i<clientCount; ++i) {
		/* TODO: Get the proper florist index to a variable with more meaningful name */
		a = rand()%3;
		
		pthread_mutex_lock(&mutexes[a]);
		QueueOffer(workQueue[a], clients[i]);
		pthread_cond_signal(&conditionVariables[a]);
		pthread_mutex_unlock(&mutexes[a]);
	}
	
	keepWorking = FALSE;
	for (i=0; i<floristCount; ++i) {
		pthread_cond_signal(&conditionVariables[i]);
		pthread_join(*(threadArray+i), NULL);
		QueueDestruct(workQueue[i]);
	}
	
	free(conditionVariables);
	free(threadArray);
	free(workQueue);
	free(florists);
	free(clients);
	free(mutexes);
	
	
	return EXIT_SUCCESS;
}



void *FloristThread(void *parameter) {
	struct Florist *info = (struct Florist*)parameter;
	struct Client currentClient;
	/*
	struct Florist info = *(struct Florist*)parameter;
	printf("Florist %s located at %d,%d with speed of %f and has %d flowers\n", info.name, info.x, info.y, info.tick, info.flowerCount);
	*/
	while (!QueueEmpty(workQueue[info->id]) || keepWorking) {
		pthread_mutex_lock(&mutexes[info->id]);
		while (QueueEmpty(workQueue[info->id]) && keepWorking)
			pthread_cond_wait(&conditionVariables[info->id], &mutexes[info->id]);
		
		currentClient = QueuePoll(workQueue[info->id]);
		pthread_mutex_unlock(&mutexes[info->id]);
		
		/* TODO: Causes last clients to be skipped before processing */
		if (keepWorking) {
			printf("%s serving to %s for %s\n", info->name, currentClient.name, currentClient.request);
		}
	}

	printf("Florist %d out\n", info->id);
	return NULL;
}



void ParseFile(char *fileName, struct Florist **florists, struct Client **clients, int *floristCount, int *clientCount) {
	int flag = FALSE;
	int index = ZERO;
	int clientIndex = ZERO;
	int floristIndex = ZERO;
	char *lineToken = NULL;
	char line[LINE_LENGTH];
	FILE *filePtr = NULL;
	
	
	*clientCount = ZERO;
	*floristCount = ZERO;
	filePtr = fopen(fileName, READ_MODE);
	if (filePtr == NULL) {
		fprintf(stderr, "\nSystem Error\nData file couldn't opened\n%s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	
	while (fgets(line, LINE_LENGTH, filePtr) != NULL) {
		if (strlen(line) == strlen(EMPTY_STRING))
			flag = TRUE;
		else if (flag)
			++(*clientCount);	
		else
			++(*floristCount);
	}
	
	
	*florists = malloc(sizeof(struct Florist) * (*floristCount));
	*clients = malloc(sizeof(struct Client) * (*clientCount));
	fseek(filePtr, SEEK_SET, ZERO);
	
	
	while (fgets(line, LINE_LENGTH, filePtr) != NULL) {
		if (strlen(line) == strlen(EMPTY_STRING)) {
			break;
		}
		
		index = ZERO;
		(*florists)[floristIndex].id = floristIndex;
		(*florists)[floristIndex].flowerCount = ZERO;
		lineToken = strtok(line, DATA_DELIMITERS);
		do {
			switch(index++) {
				case 0: strcpy((*florists)[floristIndex].name, lineToken); break;
				case 1: (*florists)[floristIndex].x = atoi(lineToken); break;
				case 2: (*florists)[floristIndex].y = atoi(lineToken); break;
				case 3: (*florists)[floristIndex].tick = atof(lineToken); break;
				default: strcpy((*florists)[floristIndex].flowers[(*florists)[floristIndex].flowerCount++], lineToken); break;
			}
			
			lineToken = strtok(NULL, DATA_DELIMITERS);
		} while (lineToken != NULL);
		++floristIndex;
	}
	
	while (fgets(line, LINE_LENGTH, filePtr) != NULL) {
		sscanf(line, "%s (%d,%d): %s", (*clients)[clientIndex].name, &(*clients)[clientIndex].x, &(*clients)[clientIndex].y, (*clients)[clientIndex].request);
		++clientIndex;
	}
	
	
	strcpy((*clients)[clientIndex].name, EMPTY_STRING);
	strcpy((*florists)[floristIndex].name, EMPTY_STRING);
	fclose(filePtr);
}
