#include <stdio.h>
#include <stdlib.h>



#define REQUIRED_ARGC 7
#define ARGUMENT_SEQUENCE_LENGTH_C "-N"
#define ARGUMENT_SEQUENCE_LENGTH_L "-n"
#define ARGUMENT_MAXIMUM_NUMBER_C "-M"
#define ARGUMENT_MAXIMUM_NUMBER_L "-m"
#define ARGUMENT_FILE_NAME_C "-X"
#define ARGUMENT_FILE_NAME_L "-x"



int main(int argc, char *argv[]) {
	
	/* Argument count check to print usage */
	if (argc != REQUIRED_ARGC) {
		printf("Usage: %s -M number -N number -X text\n", argv[0]);
		printf(" -m\tSpecifies the maximum number of sequences file can store\n");
		printf(" -n\tSpecifies the number of random numbers will be produced for a sequence\n");
		printf(" -x\tSpecifies the file to be used as communication tool\n");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}
