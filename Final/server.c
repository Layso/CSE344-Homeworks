#define _GNU_SOURCE
#include "server.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>



#define ZERO 0
#define TRUE 1
#define FALSE 0
#define ERROR_CODE -1
#define REQUIRED_ARGC 4
#define INDEX_EXEC 0
#define INDEX_PORT 1
#define INDEX_DATA 2
#define INDEX_LOG 3
#define STRING_LENGTH 100
#define READ_MODE "r"
#define NULL_CHARACTER '\0'
#define LOCAL_IP_ADDRESS "127.0.0.1"
#define DEFAULT_OPTIONS 0
#define CLIENT_QUEUE_LIMIT 25







int working;
Queue **queues;
pthread_cond_t *conds;
pthread_mutex_t *mutexes;



int main(int argc, char **argv) {
	int i;
	int portNo;
	int serverSocket;
	int providerCount;
	char *logFileName = NULL;
	char *dataFileName = NULL;
	struct Provider *providers = NULL;
	struct sigaction action;
	pthread_t *tids;
	
	
	
	/* Checking argument count to print usage in case of error */
	if (argc != REQUIRED_ARGC) {
		fprintf(stderr, "\nError! Invalid argument count\nUsage: %s [portNo] [dataFile] [logFile]\n", argv[INDEX_EXEC]);
		return EXIT_FAILURE;
	}
	
	/* Assigning arguemnts to respective variables for further usage */
	else {
		portNo = atoi(argv[INDEX_PORT]);
		logFileName = argv[INDEX_LOG];
		dataFileName = argv[INDEX_DATA];
	}
	
	
	/* Parsing data file to get providers */
	if (ParseFile(dataFileName, &providers, &providerCount) == ERROR_CODE) {
		free(providers);
		return EXIT_FAILURE;
	}
	
	/* Allocationg space for arrays and initializing variables */
	working = TRUE;
	memset(&action, ZERO, sizeof(action));
	queues = calloc(providerCount, sizeof(Queue*));
	tids = calloc(providerCount, sizeof(pthread_t));
	conds = calloc(providerCount, sizeof(pthread_cond_t));
	mutexes = calloc(providerCount, sizeof(pthread_mutex_t));
	
	
	/* Setting signal handler for SIGINT */
	action.sa_handler = SignalHandler;
	sigaction(SIGINT, &action, NULL);
	
	
	/* Initializing queue, mutex and a condition variable for each provider */
	for (i=0; i<providerCount; ++i) {
		QueueInitialize(&queues[i]);
		if (pthread_mutex_init(&mutexes[i], NULL) == ERROR_CODE) {			
			fprintf(stderr, "\nSystem Error!\nMutex initializiation failed\n%s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		
		if (pthread_cond_init(&conds[i], NULL) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error!\nCondition variable initializiation failed\n%s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		
		if (pthread_create(&tids[i], NULL, ProviderFunction, &providers[i]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error!\nThread initializiation failed\n%s\n", strerror(errno));
			return EXIT_FAILURE;
		}
	}
	
	
	/* If socket creation is succesfull, accept any connection until termination signal arrives */
	serverSocket = CreateConnection(portNo);
	if (serverSocket != ERROR_CODE) {
		while (working) {
			/* TODO:
			Accept client
			Create new thread to communicate with client
			*/
		}
	}
	
	
	/* Getting locks and declaring server is shutting down */
	for (i=0; i<providerCount; ++i) {
		pthread_mutex_lock(&mutexes[i]);
	}
	working = FALSE;
	for (i=0; i<providerCount; ++i) {
		pthread_mutex_unlock(&mutexes[i]);
	}
	
	/* Destroying elements of each provider */
	for (i=0; i<providerCount; ++i) {
		/* Joining thread before taking actions against it */
		pthread_cond_signal(&conds[i]);
		pthread_join(tids[i], NULL);
		
		/* Destroying queue, mutex and condition variable of the provider */
		QueueDestruct(queues[i]);
		pthread_cond_destroy(&conds[i]);
		pthread_mutex_destroy(&mutexes[i]);
	}
	
	/* TODO:
	Log the statistics
	*/
	
	/* Deallocating dynamic variables */
	free(tids);
	free(conds);
	free(queues);
	free(mutexes);
	free(providers);
	
	return EXIT_SUCCESS;
}



void *ProviderFunction(void *param) {
	struct Provider info = *(struct Provider*)param;
	int status;
	int newClient;
	int i = info.id;
	int timedOut = FALSE;
	struct timespec wakeTime;
	
	
	/* Setting time of waking up */
	wakeTime.tv_sec = time(NULL) + info.duration;
	wakeTime.tv_nsec = 0;
	printf("Name:%s ID:%d Perf:%d Price:%d Dura:%d\n", info.name, info.id, info.performance, info.price, info.duration);
	
	
	/* Stay signed in until either server shuts down or duration is up */
	while(working && !timedOut) {
		pthread_mutex_lock(&mutexes[i]);
		while (working && !timedOut && QueueEmpty(queues[i])){
			printf("%s sleeping\n", info.name);
			status = pthread_cond_timedwait(&conds[i], &mutexes[i], &wakeTime);
			if (status == ETIMEDOUT) {
				timedOut = TRUE;
			}
		}
		
		/* If server is still working and not timedout, get new client socket and release mutex */
		if (!timedOut && working) {
			newClient = QueuePoll(queues[i]);
		}		
		pthread_mutex_unlock(&mutexes[i]);
		
		
		if (timedOut) {
			fprintf(stderr, "Provider %s logging off due to time out\n", info.name);
		}
		
		else if (!working) {
			fprintf(stderr, "Provider %s logging off due to server shutting down\n", info.name);
		}
		
		else {
			
		}
	}
	
	
	return NULL;
}



int CreateConnection(int portNo) {
	int sock;
	struct sockaddr_in address;
	
	
	/* Perparing socket address */
	address.sin_port = portNo;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(LOCAL_IP_ADDRESS);
	
	/* Creating socket */
	sock = socket(AF_INET, SOCK_STREAM, DEFAULT_OPTIONS);
	if (sock == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nSocket creation faield\n%s\n", strerror(errno));
		return ERROR_CODE;
	}
	
	/* Binding socket to server address */
	if (bind(sock, &address, sizeof(address)) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nSocket address binding failed\n%s\n", strerror(errno));
		return ERROR_CODE;
	}
	
	/* Setting socket as passive socket */
	if (listen(sock, CLIENT_QUEUE_LIMIT) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nListening server socket failed\n%s\n", strerror(errno));
		return ERROR_CODE;
	}
	
	
	return sock;
}



void SignalHandler(int signo) {
	working = FALSE;
}



int ParseFile(const char *fileName, struct Provider **providerArray, int *arrayLength) {
	int i = ZERO;
	char *line = NULL;
	FILE *filePtr = NULL;
	char newName[STRING_LENGTH];
	char newPrice[STRING_LENGTH];
	char newDuration[STRING_LENGTH];
	char newPerformance[STRING_LENGTH];
	size_t lineLength = STRING_LENGTH;
	
	
	/* Initializing variables and allocating memory for array and buffer */
	*arrayLength = ZERO;
	line = calloc(lineLength, sizeof(char));
	*providerArray = calloc(ZERO, sizeof(struct Provider));
	memset(line, ZERO, lineLength);
	
	/* Opening input file */
	filePtr = fopen(fileName, READ_MODE);
	if (filePtr == NULL) {
		free(line);
		fprintf(stderr, "\nSystem Error!\nProvider data file %s couldn't opened\n%s\n", fileName, strerror(errno));
		return ERROR_CODE;
	}
	
	
	/* Parsing file to construct provider array */
	while (getline(&line, &lineLength, filePtr) > ZERO) {
		/* Reading line with given convention */
		sscanf(line, "%s %s %s %s\n", newName, newPerformance, newPrice, newDuration);
		
		
		/* If information fields are valid (not zero or negative) increase size and insert new element to array */
		if (atoi(newPrice) > ZERO && atoi(newDuration) > ZERO && atoi(newPerformance) > ZERO) {
			++(*arrayLength);
			*providerArray = realloc(*providerArray, (*arrayLength) * sizeof(struct Provider));
			
			strcpy((*providerArray)[*arrayLength-1].name, newName);
			(*providerArray)[*arrayLength-1].id = i++;
			(*providerArray)[*arrayLength-1].price = atoi(newPrice);
			(*providerArray)[*arrayLength-1].duration = atoi(newDuration);
			(*providerArray)[*arrayLength-1].performance = atoi(newPerformance);
		}
	}
	
	
	/* Deallocating dynamic variables */
	free(line);
	fclose(filePtr);
	
	return EXIT_SUCCESS;
}

