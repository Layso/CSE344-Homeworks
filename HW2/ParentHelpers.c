#include "ParentHelpers.h"



/* Allocates space for sequence then fills it  with randomly generated real numbers */
void ProduceSequence(int n, double **sequence) {
	int i;
	int nominator;
	int denominator;
	
	
	/* Space allocation for sequence container */
	*sequence = malloc(n * sizeof(double));
	memset(*sequence, ZERO, n * sizeof(double));
	
	/* Filling container with random real numbers */
	for (i = 0; i<n; ++i) {
		nominator = rand();
		denominator = rand();
		(*sequence)[i] = (float)nominator/(float)denominator;
	}
}



/* Write the sequence given by parameter to file */
void WriteToFile(int fileDescriptor, int n, double *sequence) {
	int i;
	size_t byteCount;
	
	
	for (i = 0; i<n; ++i) {
		byteCount = write(fileDescriptor, &sequence[i], sizeof(double));
		if (byteCount != sizeof(double)) {
			fprintf(stderr, "\nSystem Error!\nCouldn't write sequence element to file\nError message: '%s'\n", strerror(errno));
			raise(SIGINT);
			break;
		}
	}
}



/* Function to log parent operation */
void ParentLogger(int line, int n, double *sequence) {
	int i;
	int fileDescriptor;
	char string[STRING_LENGTH];
	char loggerName[STRING_LENGTH];
	
	
	/* Clearing string */
	memset(string, ZERO, STRING_LENGTH);
	memset(loggerName, ZERO, STRING_LENGTH);
	
	
	/* Preparing message string */
	sprintf(string, "Process A: producing line %d :(", line);
	for (i=0; i<n; ++i) {
		if (i+1 < n)
			sprintf(string+strlen(string), "%.2f - ", sequence[i]);
	
		else
			sprintf(string+strlen(string), "%.2f)\n", sequence[i]);
	}
	
	/* Preparing logger file name */
	sprintf(loggerName, "processA_%d.log", getpid());
	
	
	/* Printing to both stdout and log file */
	printf("%s", string);
	fileDescriptor = open(loggerName, O_WRONLY | O_APPEND | O_CREAT, FILE_PERMISSONS);
	if (fileDescriptor == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error! (Process still runs)\nLogger file couldn't opened by process A file for line %d: %s\nError message: %s\n", line, loggerName, strerror(errno));
	}
	
	else {
		if (write(fileDescriptor, string, strlen(string)) != strlen(string)) {
			fprintf(stderr, "\nSystem Error! (Process still runs)\nProcess A couldn't write log message to log file for line %d: %s\nError message: %s\n", line, loggerName, strerror(errno));
		}
		
		close(fileDescriptor);
	}
}