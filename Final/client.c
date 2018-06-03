#include "client.h"
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
#define REQUIRED_ARGC 6
#define INDEX_EXEC 0
#define INDEX_NAME 1
#define INDEX_PRIORITY 2
#define INDEX_HOMEWORK 3
#define INDEX_IP 4
#define INDEX_PORT 5
#define DEFAULT_OPTIONS 0
#define PRIORITY_COST 'C'
#define PRIORITY_PERFORMANCE 'Q'
#define PRIORITY_SPEED 'T'
#define RESPONSE_ERROR_TERMINATED 1
#define RESPONSE_ERROR_NOT_AVAILABLE 2
#define RESPONSE_ERROR_NO_PROVIDERS 3
#define RESPONSE_ERROR_MAXIMUM_THREAD 4
#define RESPONSE_ERROR_PROVIDER_LOG_OFF 5



int main(int argc, char **argv) {
	char *ip;
	char *name;
	char priority;
	int port;
	int homework;
	int server;
	struct Request request;
	struct Response response;
	

	/* Checking argument count to print usage in case of error */
	if (argc != REQUIRED_ARGC) {
		fprintf(stderr, "\nError!\nInvalid argument count\nUsage: %s [name] [priority] [homeWork] [ip] [portNo]\n", argv[INDEX_EXEC]);
		return EXIT_FAILURE;
	}

	else {
		ip = argv[INDEX_IP];
		name = argv[INDEX_NAME];
		port = atoi(argv[INDEX_PORT]);
		priority = argv[INDEX_PRIORITY][ZERO];
		homework = atoi(argv[INDEX_HOMEWORK]);
	}
	
	if (priority != PRIORITY_COST && priority != PRIORITY_SPEED && priority != PRIORITY_PERFORMANCE) {
		fprintf(stderr, "\nError!\nInvalid priority type: %c\nUse:\nQ for performance\nT for speed\nC for cost\n", priority);
		return EXIT_FAILURE;
	}
	
	server = CreateConnection(ip, port);
	if (server == ERROR_CODE) {
		return ERROR_CODE;
	}
	
	strcpy(request.name, name);
	request.homework = homework;
	request.priority = priority;
	
	printf("Client %s is requesting %c %d from server %s:%d\n", name, priority, port, ip, port);
	if (SendAll(server, &request, sizeof(request)) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error\nFailed to send client request\n%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if (ReceiveAll(server, &response, sizeof(response)) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error\nFailed to read server response\n%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if (response.status != ERROR_CODE) {
		printf("%s's task completed by %s in %d.%d seconds, cos(%d)=%f, cost is %dTL\n", name, response.providerName, response.seconds, response.miliSeconds, homework, response.result, response.cost);
	}
	
	else {
		switch (response.errorCode) {
			case RESPONSE_ERROR_TERMINATED: printf("%s's task cos(%d) couldn't completed due to server termination\n", name, homework); break;
			case RESPONSE_ERROR_NO_PROVIDERS: printf("%s's task cos(%d) couldn't completed due to no online provider left on server\n", name, homework); break;
			case RESPONSE_ERROR_NOT_AVAILABLE: printf("%s's task cos(%d) couldn't completed due to all providers are full at the moment\n", name, homework); break;
			case RESPONSE_ERROR_MAXIMUM_THREAD: printf("%s's task cos(%d) couldn't completed due to server thread limit exceeded\n", name, homework); break;
			case RESPONSE_ERROR_PROVIDER_LOG_OFF: printf("%s's task cos(%d) couldn't completed due assigned provider logged off\n", name, homework); break;
		}
	}
	
	
	return EXIT_SUCCESS;
}



int CreateConnection(char *ip, int port) {
	int serverSocket;
	struct sockaddr_in address;
	
	
	/* Creating socket */
	serverSocket = socket(AF_INET, SOCK_STREAM, DEFAULT_OPTIONS);
	if (serverSocket == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nSocket creation failed\n%s\n", strerror(errno));
		return ERROR_CODE;
	}
	
	/* Filling socket address struct */
	memset(&address, ZERO, sizeof(address));
	address.sin_port = port;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip);
	
	/* Connection to server */
	if (connect(serverSocket, (struct sockaddr*)&address, sizeof(address)) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nSocket connection failed\n%s\n", strerror(errno));
		return ERROR_CODE;
	}
	
	
	return serverSocket;
}



int SendAll(int sock, struct Request *item, int size) {
	int bytes = ZERO;
	
	while (bytes < size) {
		bytes = send(sock, item+bytes, size-bytes, DEFAULT_OPTIONS);
		if (bytes == ERROR_CODE) {
			return ERROR_CODE;
		}
	}
	
	return ZERO;
}



int ReceiveAll(int sock, struct Response *item, int size) {
	int bytes = ZERO;
	
	while (bytes < size) {
		bytes = recv(sock, item+bytes, size-bytes, DEFAULT_OPTIONS);
		if (bytes == ERROR_CODE) {
			return ERROR_CODE;
		}
	}
	
	return ZERO;
}