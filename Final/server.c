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
#include <unistd.h>
#include <time.h>


#define ZERO 0
#define TRUE 1
#define FALSE 0
#define DOUBLE 2
#define ERROR_CODE -1
#define REQUIRED_ARGC 4
#define INDEX_EXEC 0
#define INDEX_PORT 1
#define INDEX_DATA 2
#define INDEX_LOG 3
#define PI 3.14
#define TAYLOR_N 15
#define ONE 1
#define HALF_A_DEGREE 180
#define STRING_LENGTH 100
#define READ_MODE "r"
#define NULL_CHARACTER '\0'
#define LOCAL_IP_ADDRESS "127.0.0.1"
#define DEFAULT_OPTIONS 0
#define CLIENT_QUEUE_LIMIT 25
#define ARRAY_SIZE 2000
#define PRIORITY_COST 'C'
#define PRIORITY_PERFORMANCE 'Q'
#define PRIORITY_SPEED 'T'
#define SLEEP_TIME_SEC_BASE 5
#define SLEEP_TIME_MSEC_BASE 0
#define SLEEP_TIME_SEC_INTERVAL 10
#define SLEEP_TIME_MSEC_INTERVAL 100
#define MICRO_TO_MILI 1000
#define RESPONSE_ERROR_TERMINATED 1
#define RESPONSE_ERROR_NOT_AVAILABLE 2
#define RESPONSE_ERROR_NO_PROVIDERS 3
#define RESPONSE_ERROR_MAXIMUM_THREAD 4
#define RESPONSE_ERROR_PROVIDER_LOG_OFF 5


int working;
Queue **queues;
int providerCount;
int currentProviders;
pthread_mutex_t countMutex;
pthread_mutex_t mutexMutex;
char *logFileName = NULL;
pthread_cond_t *conds = NULL;
struct Statistic *stats = NULL;
pthread_mutex_t *mutexes = NULL;
struct Provider *providers = NULL;




int main(int argc, char **argv) {
	int i;
	int client;
	int portNo;
	int tidSize;
	int serverSocket;
	int *threadParam;
	char *dataFileName = NULL;
	socklen_t socketLength;
	pthread_t tids[ARRAY_SIZE];
	struct sigaction action;
	struct sockaddr_in address;
	struct Response response;
	
	
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
	
	printf("Logs will be kept at %s\n", logFileName);
	printf("%d provider threads will be created\n", providerCount);
	printf("Name\tPerformance\tPrice\tDuration\t\n");
	for (i=0; i<providerCount; ++i) {
		printf("%s\t\t%d\t%d\t%d\n", providers[i].name, providers[i].performance, providers[i].price, providers[i].duration);
	}
	
	/* Allocationg space for arrays and initializing variables */
	srand(time(NULL));
	working = TRUE;
	tidSize = ZERO;
	currentProviders = ZERO;
	queues = calloc(providerCount, sizeof(Queue*));
	conds = calloc(providerCount, sizeof(pthread_cond_t));
	stats = calloc(providerCount, sizeof(struct Statistic));
	mutexes = calloc(providerCount, sizeof(pthread_mutex_t));
	memset(&action, ZERO, sizeof(action));
	memset(stats, ZERO, providerCount * sizeof(struct Statistic));
	
	
	/* Setting signal handler for SIGINT */
	action.sa_handler = SignalHandler;
	sigaction(SIGINT, &action, NULL);
	
	
	/* Initializing queue, mutex and a condition variable for each provider */
	pthread_mutex_init(&countMutex, NULL);
	pthread_mutex_init(&mutexMutex, NULL);
	for (i=0; i<providerCount; ++i) {
		providers[i].id = i;
		QueueInitialize(&queues[i]);
		
		if (pthread_mutex_init(&mutexes[i], NULL) == ERROR_CODE) {			
			fprintf(stderr, "\nSystem Error!\nMutex initializiation failed\n%s\n", strerror(errno));
			LogStats(EXIT_FAILURE);
			return EXIT_FAILURE;
		}
		
		if (pthread_cond_init(&conds[i], NULL) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error!\nCondition variable initializiation failed\n%s\n", strerror(errno));
			LogStats(EXIT_FAILURE);
			return EXIT_FAILURE;
		}
		
		if (pthread_create(&tids[tidSize++], NULL, ProviderFunction, &providers[i].id) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error!\nThread initializiation failed\n%s\n", strerror(errno));
			LogStats(EXIT_FAILURE);
			return EXIT_FAILURE;
		}
	}
	
		
	/* If socket creation is succesfull, accept any connection until termination signal arrives */
	serverSocket = CreateConnection(portNo, &address);
	socketLength = sizeof(address);
	if (serverSocket != ERROR_CODE) {
		printf("Server is waiting connections at port %d\n", portNo);
		while (working) {
			client = accept(serverSocket, &address, &socketLength);
			if (client == ERROR_CODE) {
				if (errno != EINTR) {
					fprintf(stderr, "\nSystem Error!\nClient socket arrival failed\n%s\n", strerror(errno));
					LogStats(EXIT_FAILURE);
					return EXIT_FAILURE;
				}
				
				else {
					printf("\nTermination signal recieved\n");
					printf("Terminating all clients\n");
				}
			}
			
			else {
				/* If there is no provider left or array size exceeded send an error code */
				if (currentProviders == ZERO || tidSize == ARRAY_SIZE) {
					response.status = ERROR_CODE;
					response.errorCode = currentProviders == ZERO ? RESPONSE_ERROR_NO_PROVIDERS : RESPONSE_ERROR_MAXIMUM_THREAD;
					
					
					if (SendAll(client, &response, sizeof(response)) == ERROR_CODE) {
						fprintf(stderr, "\nSystem Error\nWriting back the response failed for main thread\n%s\n", strerror(errno));
						LogStats(EXIT_FAILURE);
						return EXIT_FAILURE;
					}
					
					close(client);
				}
				
				/* Else create a redirector thread to assign client to a privoder */
				else {
					threadParam = malloc(sizeof(client));
					*threadParam = client;
					if (pthread_create(&tids[tidSize++], NULL, RedirectorFunction, threadParam) == ERROR_CODE) {
						fprintf(stderr, "\nSystem Error!\nThread initializiation failed for redirector\n%s\n", strerror(errno));
						LogStats(EXIT_FAILURE);
						return EXIT_FAILURE;
					}
				}
			}
		}
	}
	
	else {
		working = FALSE;
	}
	
	printf("Terminating all providers\n");
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
	
	/* Joining the redirector threads */
	pthread_mutex_destroy(&countMutex);
	pthread_mutex_destroy(&mutexMutex);
	for (; i<tidSize; ++i) {
		pthread_join(tids[i], NULL);
	}
	
	
	shutdown(serverSocket, SHUT_RDWR);
	LogStats(EXIT_SUCCESS);
	printf("Statistics\nName\t\tNumber of Clients\n");
	for (i=0; i<providerCount; ++i) {
		printf("%s\t\t\t%d\n", providers[i].name, stats[i].count);
	}
	
	/* Deallocating dynamic variables */
	free(conds);
	free(stats);
	free(queues);
	free(mutexes);
	free(providers);
	
	return EXIT_SUCCESS;
}



void *ProviderFunction(void *param) {
	int i = *(int*)param;
	int status;
	int timedOut = FALSE;
	struct Work request;
	struct timespec wakeTime;
	struct Response response;
	
	
	
	/* Setting time of waking up */
	wakeTime.tv_sec = time(NULL) + providers[i].duration;
	wakeTime.tv_nsec = 0;
	
	
	/* Getting lock and declaring the provider is online */
	if (pthread_mutex_lock(&countMutex) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nMutex lock failed to increase provider count\n%s\n", strerror(errno));
		LogStats(EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}
	
	++currentProviders;
	providers[i].online = TRUE;
	
	if (pthread_mutex_unlock(&countMutex) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nMutex unlock failed for count mutex after increase\n%s\n", strerror(errno));
		LogStats(EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}
	
	
	
	/* Stay signed in until either server shuts down or duration is up */
	while(working && !timedOut) {
		if (pthread_mutex_lock(&mutexes[i]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error!\nMutex lock failed for provider %s\n%s\n", providers[i].name, strerror(errno));
			LogStats(EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}
		
		/* Wait until a client arrives or termination time comes */
		printf("Provider %s is waiting for a task\n", providers[i].name);
		while (working && !timedOut && QueueEmpty(queues[i])){
			status = pthread_cond_timedwait(&conds[i], &mutexes[i], &wakeTime);
			if (status == ETIMEDOUT) {
				timedOut = TRUE;
			}
		}
		
		/* If server is still working and not timedout, get new client socket and release mutex */
		if (!timedOut && working) {
			request = QueuePoll(queues[i]);
			fprintf(stderr, "Provider %s is processing task number %d: %d\n", providers[i].name, ++(stats[i].count), request.request.homework);
		}
		
		if (pthread_mutex_unlock(&mutexes[i]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error!\nMutex unlock failed for provider %s\n%s\n", providers[i].name, strerror(errno));
			LogStats(EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}
		
		
		/* If time is up or server termination happened, declare the offline status of provider and notify it to the remaining clients */
		if (!timedOut && working) {
			/* Clearing and resetting the response */
			memset(&response, ZERO, sizeof(response));
			strcpy(response.providerName, providers[i].name);
			response.status = ZERO;
			response.errorCode = ZERO;
			response.cost = providers[i].price;
			response.result = DoHomework(request.request.homework);
			response.seconds = (rand() % SLEEP_TIME_SEC_INTERVAL) + SLEEP_TIME_SEC_BASE;
			response.miliSeconds = (rand() % SLEEP_TIME_MSEC_INTERVAL) + SLEEP_TIME_MSEC_BASE;
			
			/* Simulating doing the homework by sleeping */
			sleep(response.seconds);
			usleep(response.miliSeconds * MICRO_TO_MILI);
			printf("Provider %s completed task #%d: cos(%d)=%f in %d.%d seconds\n", providers[i].name, stats[i].count, request.request.homework, response.result, response.seconds, response.miliSeconds);
			
			/* Writing back the response to the client */
			if (SendAll(request.clientSocket, &response, sizeof(response)) == ERROR_CODE) {
				fprintf(stderr, "\nSystem Error\nWriting back the response failed for provider %s\n%s\n", providers[i].name, strerror(errno));
				LogStats(EXIT_FAILURE);
				exit(EXIT_FAILURE);
			}
			
			/* Updating stats */
			stats[i].timeSecs += response.seconds;
			stats[i].timeMiliSecs += response.miliSeconds;
			close(request.clientSocket);
		}
	}
	
	/* If time is up or server termination happened, declare the offline status of provider and notify it to the remaining clients */
	if (timedOut || !working) {
		if (pthread_mutex_lock(&countMutex) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error!\nMutex lock failed to decrease provider count\n%s\n", strerror(errno));
			LogStats(EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}
		
		--currentProviders;
		providers[i].online = FALSE;
		
		if (pthread_mutex_unlock(&countMutex) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error!\nMutex unlock failed for count mutex after decrease\n%s\n", strerror(errno));
			LogStats(EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}
		
		memset(&response, ZERO, sizeof(response));
		response.status = ERROR_CODE;
		response.errorCode = timedOut ? RESPONSE_ERROR_PROVIDER_LOG_OFF : RESPONSE_ERROR_TERMINATED;
		while (!QueueEmpty(queues[i])) {
			request = QueuePoll(queues[i]);
			if (SendAll(request.clientSocket, &response, sizeof(response)) == ERROR_CODE) {
				fprintf(stderr, "\nSystem Error\nWriting back the response failed for provider %s\n%s\n", providers[i].name, strerror(errno));
				LogStats(EXIT_FAILURE);
				exit(EXIT_FAILURE);
			}
			
			close(request.clientSocket);
		}
		
		if (timedOut) {
			printf("Provider %s logging off due to time out\n", providers[i].name);
		}
		
		else {
			printf("Provider %s logging off due to server shutting down\n", providers[i].name);
		}
	}
	
	
	return NULL;
}



int SearchForProvider(struct Request request) {
	int i;
	int optimum;
	int index = ERROR_CODE;
	
	
	optimum = request.priority == PRIORITY_PERFORMANCE ? ZERO : __INT_MAX__;
	for (i=0; i<providerCount; ++i) {
		switch(request.priority) {
			case PRIORITY_COST:
				if (!QueueIsFull(queues[i]) && providers[i].online && providers[i].price < optimum) {
					optimum = providers[i].price;
					index = i;
				}
				
				break;
			
			case PRIORITY_SPEED:
				if (!QueueIsFull(queues[i]) && providers[i].online && queues[i]->size < optimum) {
					optimum = queues[i]->size;
					index = i;
				}
				
				break;
			
			case PRIORITY_PERFORMANCE:
				if (!QueueIsFull(queues[i]) && providers[i].online && providers[i].performance > optimum) {
					optimum = providers[i].performance;
					index = i;
				}
				
				break;
		}
	}
	
	
	return index;
}



void *RedirectorFunction(void *param) {
	int client = *(int*)param;
	int i;
	int optimumIndex;
	struct Request clientRequest;
	struct Response response;
	struct Work newWork;
	free(param);
	
	
	if (ReceiveAll(client, &clientRequest, sizeof(clientRequest)) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error\nFailed to read client request\n%s\n", strerror(errno));
		LogStats(EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}
	
	for (i=0; i<providerCount; ++i) {
		if (pthread_mutex_lock(&mutexes[i]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nFailed to lock a mutex to search provider\n%s\n", strerror(errno));
			LogStats(EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}
	}
	
	
	optimumIndex = SearchForProvider(clientRequest);
	if (optimumIndex == ERROR_CODE) {
		response.status = ERROR_CODE;
		response.errorCode = RESPONSE_ERROR_NOT_AVAILABLE;	
		if (SendAll(client, &response, sizeof(response)) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nWriting back the response failed for readirector thread, %s/%c/%d\n%s\n", clientRequest.name, clientRequest.priority, clientRequest.homework,strerror(errno));
			LogStats(EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}
		
		close(client);
	}
	
	else {
		newWork.clientSocket = client;
		newWork.request = clientRequest;
		QueueOffer(queues[optimumIndex], newWork);
		printf("Client %s (%c %d) connected, forwarded to provider %s\n", clientRequest.name, clientRequest.priority, clientRequest.homework, providers[optimumIndex].name);
		if (pthread_cond_signal(&conds[optimumIndex]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nFailed to signal condition variable after adding new work to provider\n%s\n", strerror(errno));
			LogStats(EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}
	}
	
	
	for (i=0; i<providerCount; ++i) {
		if (pthread_mutex_unlock(&mutexes[i]) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nFailed to unlock mutex after searching provider\n%s\n", strerror(errno));
			LogStats(EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}
	}
	
	return NULL;
}



int SendAll(int sock, struct Response *item, int size) {
	int bytes = ZERO;
	
	while (bytes < size) {
		bytes = send(sock, item+bytes, size-bytes, DEFAULT_OPTIONS);
		if (bytes == ERROR_CODE) {
			return ERROR_CODE;
		}
	}
	
	return ZERO;
}



int ReceiveAll(int sock, struct Request *item, int size) {
	int bytes = ZERO;
	
	while (bytes < size) {
		bytes = recv(sock, item+bytes, size-bytes, DEFAULT_OPTIONS);
		if (bytes == ERROR_CODE) {
			return ERROR_CODE;
		}
	}
	
	return ZERO;
}



double DoHomework(int homework) {
	return cosine(homework);
}



void LogStats(int status) {
	int i;
	int fileDescriptor;
	
	
	fileDescriptor = open(logFileName, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fileDescriptor == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error\nLog file couldn't opened to write %s\n%s\n", logFileName, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	for (i=0; i<providerCount; ++i) {
		dprintf(fileDescriptor, "Provider %s served %d clients for total of %d.%d seconds\n", providers[i].name, stats[i].count, stats[i].timeSecs, stats[i].timeMiliSecs);
	}
	
	if (status == EXIT_FAILURE) {
		dprintf(fileDescriptor, "\nServer terminated due to an error\n");
	}
	
	else {
		dprintf(fileDescriptor, "\nServer terminated without issues\n");
	}
	
	
	close(fileDescriptor);
}



int CreateConnection(int portNo, struct sockaddr_in *addr) {
	int sock;
	struct sockaddr_in address;
	
	
	/* Perparing socket address */
	memset(&address, ZERO, sizeof(address));
	address.sin_port = portNo;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(LOCAL_IP_ADDRESS);
	*addr = address;
	
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
	
	return ZERO;
}



double power(double x, int n) {
	double result = ONE;
	int i;
	
	
	for(i=0; i<n; ++i) {
		result *= x;
	}
	
	return result;
}



int factorial(int x) {
	int initial = ONE;
	int result = ONE;
	
	
	while (initial<=x && x>ONE) {
		result *= initial;
		++initial; 
	}
	
	return result;
}



double cosine(int degree) {
	int i;
	int temp;
	double radian;
	double sumResult;
	
	
	sumResult = ZERO;
	temp = degree%HALF_A_DEGREE;
	radian = (temp*PI)/HALF_A_DEGREE;
	
	for (i=0; i<TAYLOR_N; ++i) {
		sumResult += (power(-1,i) * power(radian,(DOUBLE*i))) / (factorial((DOUBLE*i)));
	}
	
	/* Change sign according to regions */
	if (degree>180 && degree<270) {
		sumResult = -sumResult;
	}
	
	return sumResult;
}