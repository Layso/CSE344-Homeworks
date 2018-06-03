#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

#define STRING_LENGTH 100



/* Structure to hold provider informations */
struct Provider {
	char name[STRING_LENGTH];
	int id;
	int price;
	int online;
	int duration;
	int performance;
};

/* Structure to hold provider statistics */
struct Statistic {
	char name[STRING_LENGTH];
	int count;
	int timeSecs;
	int timeMiliSecs;
};

/* Structure to hold client requests */
struct Request {
    char name[STRING_LENGTH];
	char priority;
	int homework;
};

/* Structure to hold provider work information */
struct Work {
	int clientSocket;
	struct Request request;
};

/* Structure to hold response of server */
struct Response {
	char providerName[STRING_LENGTH];
	int cost;
	int status;
	int seconds;
	int errorCode;
	int miliSeconds;
	double result;
};



int SearchForProvider(struct Request request);
int SendAll(int sock, struct Response *item, int size);
int ReceiveAll(int sock, struct Request *item, int size);
void LogStats(int status);
double DoHomework(int homework);
int CreateConnection(int portNo, struct sockaddr_in *addr);
int ParseFile(const char *fileName, struct Provider **providerArray, int *arrayLength);
void *ProviderFunction(void *param);
void *RedirectorFunction(void *param);
void SignalHandler(int signo);

#endif