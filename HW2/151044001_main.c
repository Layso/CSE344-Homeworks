#include "151044001_main.h"
#include "ParentHelpers.h"
#include "ChildHelpers.h"



int childCanCountinue;
int parentCanContinue;

int main(int argc, char *argv[]) {
	int i;
	int M;
	int N;
	int fileDescriptor;
	pid_t childPID;
	char *X;
	sigset_t sigSet;
	
	
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
	if (N <= ZERO || M <= ZERO) {
		fprintf(stderr, "Please enter values bigger than 0 as arguments\n");
		exit(EXIT_FAILURE);
	}
	
	
	/* Seeding random and initializing variables */
	srand(time(NULL));
	childCanCountinue = FALSE;
	parentCanContinue = TRUE;
	
	/* Checking file existance, quit if exists, create if doesn't */
	fileDescriptor = open(X, O_CREAT/* | O_EXCL*/, FILE_PERMISSONS);
	if (fileDescriptor == ERROR_CODE) {
		if (errno == EEXIST) {
			fprintf(stderr, "\nError!\nCommunication file already exists\nAn instance of this program might be already running\n");
		} else {
			fprintf(stderr, "\nSystem Error!\nCommunication file couldn't created: '%s'\nError message: %s\n", X, strerror(errno));
		}
		
		exit(EXIT_FAILURE);
	}
	
	if (close(fileDescriptor) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nCommunication file couldn't closed: '%s'\nError message: %s\n", X, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	
	/* Setting signals */
	memset(&sigSet, ZERO, sizeof(sigset_t));
	sigfillset(&sigSet);
	sigprocmask(SIG_BLOCK, &sigSet, NULL);
	
	
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
	int fileDescriptor;
	int currentSequenceCount;
	double *sequence = NULL;
	struct flock lockStruct;
	
	
	/* Acquire a lock on file for writing */
	memset(&lockStruct, ZERO, sizeof(struct flock));
	lockStruct.l_type = F_WRLCK;
	
	
	
	ProduceSequence(numberCount, &sequence);
	currentSequenceCount = CountSequence(fileName, numberCount, sizeof(double));
	
	
	if (currentSequenceCount == maximum) {
		parentCanContinue = FALSE;	
	}
	
	else {
		
		fileDescriptor = open(fileName, O_WRONLY | O_APPEND);
		printf("File lock parent stat: %d\n", fcntl(fileDescriptor, F_SETLKW, &lockStruct));
		perror("niye");
		WriteToFile(fileDescriptor, numberCount, sequence, maximum);
		sleep(15);
		close(fileDescriptor);
	}
	
	
	
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


void ChildHandler(int signal) {
	childCanCountinue = 1;
}

/* Starter function for child process AKA process B */
void ChildFunction(char fileName[], int numberCount) {
	int fileDescriptor;
	int currentSequenceCount;
	double *sequence = NULL;
	struct flock lockStruct;
	
	
	/* Acquire a lock on file for writing */
	memset(&lockStruct, ZERO, sizeof(struct flock));
	lockStruct.l_type = F_RDLCK;
	
	sleep(2);
	fileDescriptor = open(fileName, O_RDONLY);
	printf("File lock child stat: %d\n", fcntl(fileDescriptor, F_SETLKW, &lockStruct));
	printf("I locked it\n");
	close(fileDescriptor);
	printf("I'm done\n");
	
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



int CountSequence(char fileName[], int itemCount, int itemSize) {
	struct stat fileStat;
	int fileSize;
	
	memset(&fileStat, ZERO, sizeof(struct stat));
	stat(fileName, &fileStat);
	
	fileSize = fileStat.st_size;
	return fileSize/(itemSize*itemCount);
}



void Logger(char fileName[], char message[]) {
	int file = open(fileName, O_WRONLY | O_APPEND | O_CREAT, 0777);
	write(file, message, strlen(message));
}