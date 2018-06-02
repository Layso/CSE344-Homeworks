#ifndef SERVER_H
#define SERVER_H

#define STRING_LENGTH 100



/* Structure to hold provider informations */
struct Provider {
	char name[STRING_LENGTH];
	int id;
	int price;
	int duration;
	int performance;
};

/* Structure to hold client informations */
struct Client {
	char name[STRING_LENGTH];
	int homework;
};

/* Structure to hold provider informations */
struct Statistic {
	char name[STRING_LENGTH];
	int count;
	
};



int CreateConnection(int portNo);
int ParseFile(const char *fileName, struct Provider **providerArray, int *arrayLength);
void *ProviderFunction(void *param);
void SignalHandler(int signo);

#endif