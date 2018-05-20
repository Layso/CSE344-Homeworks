#ifndef MAIN_H
#define MAIN_H

#define ZERO 0
#define TRUE 1
#define FALSE 0
#define ERROR_CODE -1
#define REQUIRED_ARGC 2
#define LINE_LENGTH 1024
#define DATA_DELIMITERS "():;, \n"
#define EMPTY_STRING "\n"
#define READ_MODE "r"

#define STRING_LENGTH 64
#define ARRAY_LENGTH 50


struct Florist {
    int id;
	char name[STRING_LENGTH];
	int x;
	int y;
	float tick;
	int flowerCount;
	char flowers[ARRAY_LENGTH][STRING_LENGTH];	
};

struct Client {
	char name[STRING_LENGTH];
	int x;
	int y;
	char request[STRING_LENGTH];
};

struct Statistic {
	char name[STRING_LENGTH];
	int totalTime;
	int totalSales;
};



void ParseFile(char *fileName, struct Florist **florists, struct Client **clients, int *floristCount, int *clientCount);
void *FloristThread(void *parameter);

#endif