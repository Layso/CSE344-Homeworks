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
void WriteToFile(int fileDescriptor, int n, double *sequence, int maximumLines) {
	
}