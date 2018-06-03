#ifndef CLIENT_H
#define CLIENT_H

#define STRING_LENGTH 100



/* Structure to hold client requests */
struct Request {
    char name[STRING_LENGTH];
	char priority;
	int homework;
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



int SendAll(int sock, struct Request *item, int size);
int ReceiveAll(int sock, struct Response *item, int size);
int CreateConnection(char *ip, int port);

#endif