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



/*  */
void WriteToFile(int fileDescriptor, int n, double *sequence) {
	int i;
	size_t byteCount;
	
	
	for (i = 0; i<n; ++i) {
		byteCount = write(fileDescriptor, &sequence[i], sizeof(double));
		if (byteCount != sizeof(double)) {
			fprintf(stderr, "\nError!\nCouldn't write sequence element to file\n");
			raise(SIGINT);
		}
	}
}