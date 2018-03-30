#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include "main.h"



#define ZERO 0
#define TRUE 1
#define FALSE 0
#define ERROR_CODE -1
#define EMPTY_STRING ""
#define NEW_LINE_CHARACTER "\n"
#define NULL_CHARACTER "\n"
#define REQUIRED_ARGC 7
#define ARGUMENT_SEQUENCE_LENGTH_C "-N"
#define ARGUMENT_SEQUENCE_LENGTH_L "-n"
#define ARGUMENT_MAXIMUM_NUMBER_C "-M"
#define ARGUMENT_MAXIMUM_NUMBER_L "-m"
#define ARGUMENT_FILE_NAME_C "-X"
#define ARGUMENT_FILE_NAME_L "-x"



int main(int argc, char *argv[]) {
	pid_t childPID;
	char *X;
	int N;
	int M;
	int i;
	
	/* Argument count check to print usage */
	if (argc != REQUIRED_ARGC) {
		fprintf(stderr, "Usage: %s -M number -N number -X text\n", argv[0]);
		fprintf(stderr, " -m\tSpecifies the maximum number of sequences file can store\n");
		fprintf(stderr, " -n\tSpecifies the number of random numbers will be produced for a sequence\n");
		fprintf(stderr, " -x\tSpecifies the file to be used as communication tool\n");
		exit(EXIT_FAILURE);
	}
	
	/* Taking command line arguments */
	for (i=0; i<argc-1; ++i) {
		if (strcmp(argv[i], ARGUMENT_SEQUENCE_LENGTH_C) == ZERO || strcmp(argv[i], ARGUMENT_SEQUENCE_LENGTH_L) == ZERO) {
			N = atoi(argv[i+1]);
		} else if (strcmp(argv[i], ARGUMENT_MAXIMUM_NUMBER_C) == ZERO || strcmp(argv[i], ARGUMENT_MAXIMUM_NUMBER_L) == ZERO) {
			M = atoi(argv[i+1]);
		} else if (strcmp(argv[i], ARGUMENT_FILE_NAME_C) == ZERO || strcmp(argv[i], ARGUMENT_FILE_NAME_L) == ZERO) {
			X = argv[i+1];
		}
	}
	
	/* Controlling command line arguments */
	if (N == ZERO || M == ZERO || strcmp(X, EMPTY_STRING) == ZERO) {
		fprintf(stderr, "Please enter non-empty and bigger than 0 values as arguments\n");
		exit(EXIT_FAILURE);
	}
	
	/* Transform to multiprocess */
	childPID = fork();
	switch(childPID) {
		case ERROR_CODE:
			fprintf(stderr, "\nError!Fork failed.\n");
			exit(EXIT_FAILURE);
		
		case ZERO:
			ChildFunction(X, N);
			break;
		
		default:
			ParentFunction(X, M, N, childPID);
			break;
		
	}
	
	
	return 0;
}



/* Starter function for parent process AKA process A */
void ParentFunction(char fileName[], int maximum, int numberCount, pid_t childPID) {
	 printf("Process A: Ehe\n");
	 
	 /* TODO
	  * 
	  * Produce a sequence
	  * Lock the file
	  * Write the sequence
	  * Unlock the file
	  * Log the process
	  * Check if SIGINT recieved
	  */
}



/* Starter function for child process AKA process B */
void ChildFunction(char fileName[], int numberCount) {
	printf("Process B: Ehehedha\n");
	
	/* TODO
	 *
	 * Lock the file
	 * Read the sequence
	 * Unlock the file
	 * Process sequence
	 * Log the process
	 * Check if SIGINT recieved
	 */
}