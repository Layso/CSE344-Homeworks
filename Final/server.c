#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>



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



/* Structure to hold provider informations */
struct Provider {
	char name[STRING_LENGTH];
	int price;
	int duration;
	int performance;
};



int ParseFile(const char *fileName, struct Provider **providerArray, int *arrayLength);




int main(int argc, char **argv) {
	int i;
	int portNo;
	int providerCount;
	char *logFileName = NULL;
	char *dataFileName = NULL;
	struct Provider *providers = NULL;
	
	
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
	
	else {
		for (i=0; i<providerCount; ++i) {
			printf("Name: %s\nPrice: %d\nDuration: %d\nPerformance: %d\n\n", providers[i].name, providers[i].price, providers[i].duration, providers[i].performance);
		}
	}
	
	
	
	/* Deallocating dynamic variables */
	free(providers);
	
	return EXIT_SUCCESS;
}



int ParseFile(const char *fileName, struct Provider **providerArray, int *arrayLength) {
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

