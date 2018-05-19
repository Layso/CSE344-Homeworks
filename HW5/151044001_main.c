#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define ZERO 0
#define TRUE 1
#define FALSE 0
#define ERROR_CODE -1
#define REQUIRED_ARGC 2
#define STRING_LENGTH 64
#define ARRAY_LENGTH 50
#define LINE_LENGTH 1024
#define DATA_DELIMITERS "():;, \n"
#define EMPTY_STRING "\n"
#define READ_MODE "r"



struct Florist {
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


void CreateFlorists(char *fileName, struct Florist **florists, struct Client **clients);



int main(int argc, char **argv) {
	int i;
	struct Florist *florists = NULL;
	struct Client *clients = NULL;
	
	
	if (argc != REQUIRED_ARGC) {
		fprintf(stderr, "Usage: %s [fileName]\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	
	CreateFlorists(argv[1], &florists, &clients);
	for (i=0; strcmp(florists[i].name, EMPTY_STRING); ++i) {
		printf("Florist %s located at %d,%d with speed of %f and has %d flowers\n", florists[i].name, florists[i].x, florists[i].y, florists[i].tick, florists[i].flowerCount);
	}
	
	for (i=0; strcmp(clients[i].name, EMPTY_STRING); ++i) {
		printf("%s waiting for %s at location %d,%d\n", clients[i].name, clients[i].request, clients[i].x, clients[i].y);
	}
	
	free(florists);
	free(clients);
	return EXIT_SUCCESS;
}



void CreateFlorists(char *fileName, struct Florist **florists, struct Client **clients) {
	int index = ZERO;
	int clientCount = ZERO;
	int clientIndex = ZERO;
	int floristIndex = ZERO;
	int floristCount = ZERO;
	int flag = FALSE;
	char *lineToken = NULL;
	char line[LINE_LENGTH];
	FILE *filePtr = NULL;
	
	
	filePtr = fopen(fileName, READ_MODE);
	if (filePtr == NULL) {
		fprintf(stderr, "\nSystem Error\nData file couldn't opened\n%s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	
	while (fgets(line, LINE_LENGTH, filePtr) != NULL) {
		if (strlen(line) == strlen(EMPTY_STRING))
			flag = TRUE;
		else if (flag)
			++clientCount;	
		else
			++floristCount;
	}
	
	
	*florists = malloc(sizeof(struct Florist) * (floristCount+1));
	*clients = malloc(sizeof(struct Client) * (clientCount+1));
	fseek(filePtr, SEEK_SET, ZERO);
	
	
	while (fgets(line, LINE_LENGTH, filePtr) != NULL) {
		if (strlen(line) == strlen(EMPTY_STRING)) {
			break;
		}
		
		index = ZERO;
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
