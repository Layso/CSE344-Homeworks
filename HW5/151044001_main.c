#include "151044001_main.h"
#include "queue.h"



/* Global variables to reach accross threads */
Queue **workQueue = NULL;
pthread_cond_t *conditionVariables = NULL;
pthread_mutex_t *mutexes = NULL;
int keepWorking = TRUE;



int main(int argc, char **argv) {
	int i;
	int floristIndex;
	int floristCount, clientCount;
	void *returnValue = NULL;
	pthread_t *threadArray = NULL;
	struct Client *clients = NULL;
	struct Statistic *stats = NULL;
	struct Florist *florists = NULL;
	
	
	/* Controlling argument count to exit in case no filename has provided */
	if (argc != REQUIRED_ARGC) {
		fprintf(stderr, "Usage: %s [fileName]\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	
	/* Parsing file to get florists, clients and their counts */
	ParseFile(argv[1], &florists, &clients, &floristCount, &clientCount);
	
	
	/* Allocating memory for arrays */
	stats = malloc(sizeof(struct Statistic) * floristCount);
	mutexes = malloc(sizeof(pthread_mutex_t) * floristCount);
	workQueue = malloc(sizeof(Queue*) * floristCount);
	threadArray = malloc(sizeof(pthread_t) * floristCount);
	conditionVariables = malloc(sizeof(pthread_cond_t) * floristCount);
	if (conditionVariables == NULL || threadArray == NULL || workQueue == NULL || florists == NULL || clients == NULL || mutexes == NULL || stats == NULL) {
		fprintf(stderr, "\nSystem Error\nNot all memory allocations succesfully completed\nError message: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	
	/* Creating thread pool before processing clients */
	for (i=0; i<floristCount; ++i) {
		workQueue[i] = NULL;
		QueueInitialize(workQueue+i);
		
		if (pthread_mutex_init(mutexes+i, NULL) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nMutex initializiation failed\nError message: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		
		if (pthread_cond_init(conditionVariables+i, NULL) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nCondition variable initializiation failed\nError message: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		
		if (pthread_create(threadArray+i, NULL, FloristThread, florists+i) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nThread creation failed\nError message: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
	}
	printf("%d florists have been created\n", floristCount);
	
	
	/* After everything is done, for each client get closest florist to client and redirect client to proper florist*/
	for (i=0; i<clientCount; ++i) {
		/* Get proper florist index */
		floristIndex = GetProperFloristForClient(clients[i], florists, floristCount);
		
		/* If no proper florist has found print a proper message */
		if (floristIndex == ERROR_CODE) {
			fprintf(stderr, "No available florist found for %s's '%s' request\n", clients[i].name, clients[i].request);
		}
		
		/* Else a florist has found with requested flower, add client to work queue of that florist */
		else {
			if (pthread_mutex_lock(&mutexes[floristIndex]) == ERROR_CODE) {
				fprintf(stderr, "\nSystem Error\nMutex lock failed\nError message: %s\n", strerror(errno));
				return EXIT_FAILURE;
			}
			
			QueueOffer(workQueue[floristIndex], clients[i]);
			
			if (pthread_cond_signal(&conditionVariables[floristIndex]) == ERROR_CODE) {
				fprintf(stderr, "\nSystem Error\nCondition variable signal failed\nError message: %s\n", strerror(errno));
				return EXIT_FAILURE;
			}
			
			if (pthread_mutex_unlock(&mutexes[floristIndex]) == ERROR_CODE) {
				fprintf(stderr, "\nSystem Error\nMutex unlock failed\nError message: %s\n", strerror(errno));
				return EXIT_FAILURE;
			}
		}
	}
	
	
	
	/* Acquire locks to notify all clients are proceeded */
	for (i=0; i<floristCount; ++i) {
		if (pthread_mutex_lock(&mutexes[i]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nMutex lock failed\nError message: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
	}
	
	keepWorking = FALSE;
	fprintf(stderr, "All requests are proceeded\n");	
	
	/* Release the locks to let them continue work */
	for (i=0; i<floristCount; ++i) {
		if (pthread_mutex_unlock(&mutexes[i]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nMutex unlock failed\nError message: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}	
	}
	
	
	/* Wait for threads to complete their work, get florist stats and destroy florist specific allocations */
	for (i=0; i<floristCount; ++i) {
		if (pthread_cond_signal(&conditionVariables[i]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nCondition variable signal failed\nError message: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		
		if (pthread_join(*(threadArray+i), &returnValue) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nThread join failed\nError message: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		
		stats[i] = *(struct Statistic*)returnValue;
		free(returnValue);
		QueueDestruct(workQueue[i]);
		
		if (pthread_mutex_destroy(&mutexes[i]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nMutex destroy failed\nError message: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		
		if (pthread_cond_destroy(&conditionVariables[i]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nCondition variable destroy failed\nError message: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
	}
	
	
	/* Printing statistic of florists */
	printf("\nSale statistics for today:\n");
	printf("----------------------------------------------------------\n");
	printf("Florist\t\t\t# of sales\t\tTotal time\n");
	printf("----------------------------------------------------------\n");
	for (i=0; i<floristCount; ++i) {
		printf("%s\t\t\t%d\t\t\t%d\n", stats[i].name, stats[i].totalSales, stats[i].totalTime);
	}
	printf("----------------------------------------------------------\n");
	
	
	/* Releasing the allocated memory spaces */
	free(conditionVariables);
	free(threadArray);
	free(workQueue);
	free(florists);
	free(clients);
	free(mutexes);
	free(stats);
	
	
	/* Finally exit program with success code */
	return EXIT_SUCCESS;
}



void *FloristThread(void *parameter) {
	struct Florist *info = (struct Florist*)parameter;
	struct Client currentClient;
	struct Statistic *stats;
	int status;
	int random;
	
	
	/* Allocating memory for stats */
	stats = malloc(sizeof(struct Statistic));
	if (stats == NULL) {
		fprintf(stderr, "\nSystem Error\nMemory allocation for florist failed\nError message: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	strcpy(stats->name, info->name);
	stats->totalSales = ZERO;
	stats->totalTime = ZERO;
	
	
	/* Work until end signal has arrived and queue is empty */
	while (!QueueEmpty(workQueue[info->id]) || keepWorking) {
		/* Acquire lock to get client from queue */
		if (pthread_mutex_lock(&mutexes[info->id]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nMutex lock failed\nError message: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		/* If queue is empty, wait until a signal arrives that indicates there is a waiting client */
		while (QueueEmpty(workQueue[info->id]) && keepWorking) {
			if (pthread_cond_wait(&conditionVariables[info->id], &mutexes[info->id]) == ERROR_CODE) {
				fprintf(stderr, "\nSystem Error\nCondition variable wait failed\nError message: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		
		/* Try to get client from queue to process */
		currentClient = QueuePoll(workQueue[info->id], &status);
		if (pthread_mutex_unlock(&mutexes[info->id]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nMutex unlock failed\nError message: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		/* If a valid client has obtained, process it */
		if (status) {
			random = (rand() % RANDOM_INTERVAL) + RANDOM_BASE + DELIVERY_CONSTANT*(int)Distance(info->x, info->y, currentClient.x, currentClient.y);
			usleep(random*MICRO_TO_MILI);
			printf("Florist %s has delivered %s to %s in %dms\n", info->name, currentClient.request, currentClient.name, random);
			++(stats->totalSales);
			(stats->totalTime) += random;
		}
	}
	
	
	/* Printing final message as florist */
	printf("%s closing the shop\n", info->name);
	return stats;
}



int GetProperFloristForClient(struct Client client, struct Florist *florists, int floristCount) {
	int i;
	int index = ERROR_CODE;
	float distance = __DBL_MAX__;
	float currentDistance = distance;
	int x = client.x;
	int y = client.y;
	
	
	/* Searching least distanced florist that sells requested flower */
	for (i=0; i<floristCount; ++i) {
		currentDistance = Distance(florists[i].x, florists[i].y, x, y);
		if (currentDistance < distance && GotFlowerOfType(client.request, florists[i].flowers, florists[i].flowerCount)) {
			distance = currentDistance;
			index = i;
		}
	}
	
	/* If no florist found with requested flower return error code */
	return index;
}



int GotFlowerOfType(char *required, char saleList[ARRAY_LENGTH][STRING_LENGTH], int flowerCount) {
	int i;
	
	/* If florist list includes requested flower return true, else false */
	for (i=0; i<flowerCount; ++i) {
		if (!strcmp(required, saleList[i])) {
			return TRUE;
		}
	}
	
	return FALSE;
}



float Distance(int x1, int y1, int x2, int y2) {
	/* Basic Euclidean distance calculation */
	return sqrt(pow(x2 - x1, SQUARE) + pow(y2 - y1, SQUARE));
}



void ParseFile(char *fileName, struct Florist **florists, struct Client **clients, int *floristCount, int *clientCount) {
	int flag = FALSE;
	int index = ZERO;
	int clientIndex = ZERO;
	int floristIndex = ZERO;
	char *lineToken = NULL;
	char line[LINE_LENGTH];
	FILE *filePtr = NULL;
	
	
	/* Trying to open file */
	*clientCount = ZERO;
	*floristCount = ZERO;
	filePtr = fopen(fileName, READ_MODE);
	if (filePtr == NULL) {
		fprintf(stderr, "\nSystem Error\nData file couldn't opened\nError message: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	/* Counting florist and client count from file */
	while (fgets(line, LINE_LENGTH, filePtr) != NULL) {
		if (strlen(line) == strlen(EMPTY_STRING))
			flag = TRUE;
		else if (flag)
			++(*clientCount);	
		else
			++(*floristCount);
	}
	
	
	/* Allocating memory for arrays */
	*florists = malloc(sizeof(struct Florist) * (*floristCount));
	*clients = malloc(sizeof(struct Client) * (*clientCount));
	if (clients == NULL || florists == NULL) {
		fprintf(stderr, "\nSystem Error\nMemory allocation for file parse failed\nError message: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	
	/* Get florists and parse to the struct */
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
	
	/* Get clients and parse to the struct */
	while (fgets(line, LINE_LENGTH, filePtr) != NULL) {
		sscanf(line, "%s (%d,%d): %s", (*clients)[clientIndex].name, &(*clients)[clientIndex].x, &(*clients)[clientIndex].y, (*clients)[clientIndex].request);
		++clientIndex;
	}
	
	/* Close file pointer */
	fclose(filePtr);
}
