#include "ChildHelpers.h"



/* Function to read last line from file and shrink it to remove read line  */
void ReadSequence(int fileDescriptor, int n, double **sequence, int line) {
	int i;
	int offset;
	
	
	/* Allocating space for sequence array */
	*sequence = malloc(n* sizeof(double));
	memset(*sequence, ZERO, n * sizeof(double));
	
	/* Changing file offset to read from last line */
	offset = lseek(fileDescriptor, (line -1 ) * n * sizeof(double), SEEK_SET);	
	for (i=0; i<n; ++i) {
		read(fileDescriptor, &(*sequence)[i], sizeof(double));
	}
	
	/* Shrinking file size from end to remove last line */
	if (ftruncate(fileDescriptor, offset) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nCommunication file couldn't shrinked by process B\nError message: %s\n", strerror(errno));
		kill(getppid(), SIGINT);
		raise(SIGINT);
	}
}



/* Function to calculate DFT from read line */
/* Source of implementation: https://batchloaf.wordpress.com/2013/12/07/simple-dft-in-c/ */
void CalculateDFT(int n, double *sequence, double **real, double **imaginary) {
	int i, j;
	double PI2 = M_PI * 2;


    for (i=0 ; i<n ; ++i) {
        (*real)[i] = 0;
        (*imaginary)[i] = 0;
        
		for (j=0 ; j<n ; ++j) {
			(*real)[i] += sequence[j] * cos(j * i * PI2 / n);
		} 

        for (j=0 ; j<n ; ++j) {
			(*imaginary)[i] -= sequence[j] * sin(j * i * PI2 / n);
		}
    }
}



/* Function to log child operation */
void ChildLogger(int line, int n, double *sequence, double *real, double *imaginary) {
	int i;
	int fileDescriptor;
	char string[STRING_LENGTH];
	char loggerName[STRING_LENGTH];
	
	
	/* Clearing strings */
	memset(string, ZERO, STRING_LENGTH);
	memset(loggerName, ZERO, STRING_LENGTH);
	
	
	/* Preparing message string */
	sprintf(string, "Process B: the DFT of line %d (", line);
	for (i=0; i<n; ++i) {
		if (i+1 < n)
			sprintf(string+strlen(string), "%.2f - ", sequence[i]);
	
		else
			sprintf(string+strlen(string), "%.2f) is:\t", sequence[i]);
	}
	
	for (i=0; i<n; ++i) {
		sprintf(string+strlen(string), "(%.2f)+", real[i]);
		sprintf(string+strlen(string), "(%.2f)i ", imaginary[i]);
	}
	
	sprintf(string+strlen(string), "\n");
	/* Preparing logger file name */
	sprintf(loggerName, "processB_%d.log", getpid());
	
	
	/* Printing to both stdout and log file */
	printf("%s", string);
	fileDescriptor = open(loggerName, O_WRONLY | O_APPEND | O_CREAT, FILE_PERMISSONS);
	if (fileDescriptor == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error! (Process still runs)\nLogger file couldn't opened by process B for line %d: %s\nError message: %s\n", line, loggerName, strerror(errno));
	}
	
	else {
		if (write(fileDescriptor, string, strlen(string)) != strlen(string)) {
			fprintf(stderr, "\nSystem Error! (Process still runs)\nProcess B couldn't write log message to log file for line %d: %s\nError message: %s\n", line, loggerName, strerror(errno));
		}
		
		close(fileDescriptor);
	}
}