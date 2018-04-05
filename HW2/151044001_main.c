#include "151044001_main.h"
#include "ParentHelpers.h"
#include "ChildHelpers.h"



int exitFlag;



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
	exitFlag = FALSE;
	
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
			fprintf(stderr, "\nSystem Error!\nFork failed\nError message: '%s'\n", strerror(errno));
			exit(EXIT_FAILURE);
		
		case ZERO:
			ChildFunction(X, M, N);
			break;
		
		default:
			ParentFunction(X, M, N, childPID);
			break;
		
	}
	
	
	return 0;
}



/* Starter function for parent process AKA process A */
void ParentFunction(char fileName[], int maximum, int itemCount, pid_t childPID) {
	int lineCount;
	int fileDescriptor;
	double *sequence;
	struct flock lockStruct;
	struct sigaction sigAction;
	sigset_t maskSet, pendingSet;
	
	
	/* Clear structs */
	memset(&maskSet, ZERO, sizeof(sigset_t));
	memset(&pendingSet, ZERO, sizeof(sigset_t));
	memset(&sigAction, ZERO, sizeof(struct sigaction));
	memset(&lockStruct, ZERO, sizeof(struct flock));
	lockStruct.l_type = F_WRLCK;
	
	
	/* Assign a handler to prevent program shut down on arrival */ 
	sigAction.sa_handler = EmptyHandler;
	if (sigaction(SIGUSR1, &sigAction, NULL) == ERROR_CODE && !exitFlag) {
		fprintf(stderr, "\nSystem Error!\nSignal handler couldn't assigned by process A\nError message: %s\n", strerror(errno));
		
		/* Sending both signals to child due to unknown state of child signal mask process */
		kill(childPID, SIGUSR1);
		kill(childPID, SIGINT);
		exitFlag = TRUE;
	}
	
	/* Prepare a mask that excludes communication signal */
	if (sigprocmask(SIG_SETMASK, NULL, &maskSet) == ERROR_CODE && !exitFlag) {
		fprintf(stderr, "\nSystem Error!\nSignal mask couldn't applied by process A\nError message: %s\n", strerror(errno));
		kill(childPID, SIGUSR1);
		kill(childPID, SIGINT);
		exitFlag = TRUE;
	}
	
	if (sigdelset(&maskSet, SIGUSR1) == ERROR_CODE && !exitFlag) {
		fprintf(stderr, "\nSystem Error!\nSignal set couldn't changed by process A\nError message: %s\n", strerror(errno));
		kill(childPID, SIGUSR1);
		kill(childPID, SIGINT);
		exitFlag = TRUE;
	}
	
	/* Send signal to indicate parent is ready then wait for child to be ready */
	kill(childPID, SIGUSR1);
	if (sigsuspend(&maskSet) == ERROR_CODE && !exitFlag && errno != EINTR) {
		fprintf(stderr, "\nSystem Error!\nProcess A couldn't suspended itself\nError message: %s\n", strerror(errno));
		kill(childPID, SIGUSR1);
		kill(childPID, SIGINT);
		exitFlag = TRUE;
	}
	
	
	
	/* Endless race (until SIGINT arrives) can start now */
	while(!exitFlag) {
		/* Try to open file */
		fileDescriptor = open(fileName, O_WRONLY | O_APPEND);
		if (fileDescriptor == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error!\nCommunication file couldn't opened by process A to write sequence: '%s'\nError message: %s\n", fileName, strerror(errno));
			raise(SIGINT);
		}
		
		else {
			/* Try to lock file if opened */
			if (fcntl(fileDescriptor, F_SETLKW, &lockStruct) == ERROR_CODE) {
				close(fileDescriptor);
				fprintf(stderr, "\nSystem Error!\nCommunication file couldn't locked by process A to write sequence: '%s'\nError message: %s\n", fileName, strerror(errno));
				raise(SIGINT);
			}
			
			/* If succesfully locked, start processing */
			else {
				lineCount = CountSequence(fileName, itemCount, sizeof(double));
				
				/* If file is full, politely wait for child to empty it */
				if (lineCount == maximum) {
					close(fileDescriptor);
					sigsuspend(&maskSet);
				}
				
				/* Else write a new sequence to file */
				else {
					ProduceSequence(itemCount, &sequence);
					WriteToFile(fileDescriptor, itemCount, sequence);
					close(fileDescriptor);
					
					/* Logging and clearing */
					ParentLogger(lineCount+1, itemCount, sequence);
					free(sequence);
					
					/* Indicate the politely waiting child that it can continue now */
					if (lineCount == ZERO) {
						kill(childPID, SIGUSR1);
					}
				}
			}
		}
		
		
		/* Check if SIGINT arrived to end process */
		sigpending(&pendingSet);
		if (sigismember(&pendingSet, SIGINT)) {
			exitFlag = TRUE;
			kill(childPID, SIGINT);
		}
	}
	
	
	/* Wait for child and clean file */
	wait(NULL);
	if (remove(fileName) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nCommunication file couldn't removed by process A: '%s'\nError message: %s\n", fileName, strerror(errno));
	}
}



/* Starter function for child process AKA process B */
void ChildFunction(char fileName[], int maximum, int itemCount) {
	int lineCount;
	int fileDescriptor;
	double dft;
	double *sequence;
	struct flock lockStruct;
	struct sigaction sigAction;
	sigset_t maskSet, pendingSet;
	
	
	/* Clear structs */
	memset(&maskSet, ZERO, sizeof(sigset_t));
	memset(&pendingSet, ZERO, sizeof(sigset_t));
	memset(&sigAction, ZERO, sizeof(struct sigaction));
	memset(&lockStruct, ZERO, sizeof(struct flock));
	lockStruct.l_type = F_WRLCK;
	
	
	/* Assign a handler to prevent program shut down on arrival */
	sigAction.sa_handler = EmptyHandler;
	if (sigaction(SIGUSR1, &sigAction, NULL) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nSignal handler couldn't assigned by process B\nError message: %s\n", strerror(errno));
		
		/* Sending both signals to parent due to unknown state of parent signal mask process */
		kill(getppid(), SIGUSR1);
		kill(getppid(), SIGINT);
		exitFlag = TRUE;
	}
	
	/* Prepare a mask that excludes communication signal */
	if (sigprocmask(SIG_SETMASK, NULL, &maskSet) == ERROR_CODE && !exitFlag) {
		fprintf(stderr, "\nSystem Error!\nSignal mask couldn't applied by process B\nError message: %s\n", strerror(errno));
		kill(getppid(), SIGUSR1);
		kill(getppid(), SIGINT);
		exitFlag = TRUE;
	}
	
	if (sigdelset(&maskSet, SIGUSR1) == ERROR_CODE && !exitFlag) {
		fprintf(stderr, "\nSystem Error!\nSignal set couldn't changed by process B\nError message: %s\n", strerror(errno));
		kill(getppid(), SIGUSR1);
		kill(getppid(), SIGINT);
		exitFlag = TRUE;
	}
	
	/* Wait for parent to be ready, then send signal to indicate child is ready */
	if (sigsuspend(&maskSet) == ERROR_CODE && !exitFlag && errno != EINTR) {
		fprintf(stderr, "\nSystem Error!\nProcess B couldn't suspended itself\nError message: %s\n", strerror(errno));
		kill(getppid(), SIGUSR1);
		kill(getppid(), SIGINT);
		exitFlag = TRUE;
	}
	kill(getppid(), SIGUSR1);
	
	
	
	/* Endless race (until SIGINT arrives) can start now */
	while(!exitFlag) {
		/* Try to open file */
		fileDescriptor = open(fileName, O_RDWR);
		if (fileDescriptor == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error!\nCommunication file couldn't opened by process B to read sequence: '%s'\nError message: %s\n", fileName, strerror(errno));
			raise(SIGINT);
		}
		
		else {
			/* Try to lock file if opened */
			if (fcntl(fileDescriptor, F_SETLKW, &lockStruct) == ERROR_CODE) {
				close(fileDescriptor);
				fprintf(stderr, "\nSystem Error!\nCommunication file couldn't locked by process B to read sequence: '%s'\nError message: %s\n", fileName, strerror(errno));
				kill(getppid(), SIGINT);
				raise(SIGINT);
			}
			
			/* If succesfully locked, start processing */
			else {
				lineCount = CountSequence(fileName, itemCount, sizeof(double));
				
				/* If file is full, politely wait for parent to fill it */
				if (lineCount == ZERO) {
					close(fileDescriptor);
					sigsuspend(&maskSet);
				}
				
				/* Else read new sequence from file to process */
				else {
					ReadSequence(fileDescriptor, itemCount, &sequence, lineCount);
					close(fileDescriptor);
					
					/* Processing, logging and clearing */
					dft = CalculateDFT(itemCount, sequence);
					ChildLogger(lineCount, itemCount, sequence, dft);
					free(sequence);
					
					
					/* Indicate the politely waiting parent that it can continue now */
					if (lineCount == maximum) {
						kill(getppid(), SIGUSR1);
					}
				}
			}
		}
		
		
		
		/* Check if SIGINT arrived to end process */
		sigpending(&pendingSet);
		if (sigismember(&pendingSet, SIGINT)) {
			exitFlag = TRUE;
		}
	}
	
	/* Exit */
	_exit(EXIT_SUCCESS);
}



/* An empty signal handler function to assign communication signal to prevent
   program termination when the signal arrives to programs */
void EmptyHandler(int signal) {
	/* Intentionally left empty */
}



/* Finds the line number by divison of total file size to size of a line */
int CountSequence(char fileName[], int itemCount, int itemSize) {
	struct stat fileStat;
	int fileSize;
	
	memset(&fileStat, ZERO, sizeof(struct stat));
	stat(fileName, &fileStat);
	
	fileSize = fileStat.st_size;
	return fileSize/(itemSize*itemCount);
}