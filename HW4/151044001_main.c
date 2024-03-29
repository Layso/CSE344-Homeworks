#include "151044001_main.h"



int keepWorking = TRUE;



int main(int argc, char **argv) {
	int i, j;
	int status;
	int sharedDescriptor;
	int currentChild = ZERO;
	int childList[CHEF_COUNT];
	struct sigaction signalStruct;
	SharedStructure *shared;
	
	
	
	/* Preparations */
	shared = malloc(sizeof(SharedStructure));
	memset(&signalStruct, ZERO, sizeof(struct sigaction));
	memset(shared, ZERO, sizeof(SharedStructure));
	srand(time(NULL));
	
	/* Creating signal handler to exit program gracefuly with SIGINT */
	signalStruct.sa_handler = handler;
	if (sigaction(SIGINT, &signalStruct, NULL) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nSignal handler creation failed\n%s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	
	/* Creating descriptor with the desired shared memory name */
	sharedDescriptor = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, FILE_PERMISSIONS);
	if (sharedDescriptor == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nShared memory creation failed\n%s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	
	/* Changing size of file to the size of semaphores */
	if (ftruncate(sharedDescriptor, sizeof(SharedStructure)) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nMemory size extension failed\n%s\n", strerror(errno));
		shm_unlink(SHARED_MEMORY_NAME);
		return EXIT_FAILURE;
	}
	
	/* Getting shared memory structure and initializing structure members */
	shared = (SharedStructure*)mmap(NULL, sizeof(SharedStructure), PROT_READ | PROT_WRITE, MAP_SHARED, sharedDescriptor, ZERO);
	sem_init(&(shared->working), TRUE, UNLOCKED);
	sem_init(&(shared->done), TRUE, LOCKED);
	for (i=0; i<INGREDIENT_COUNT; ++i) {
		sem_init(&(shared->ingredients[i]), TRUE, LOCKED);
	}
	
	
	/* Creating wholesaler */
	status = fork();
	switch(status) {
		case 0 : WholeSaler(sharedDescriptor); break;
		case -1: fprintf(stderr, "\nSystem Error\nFork failed for wholesaler\n%s\n", strerror(errno));
				 for (i=0; i<INGREDIENT_COUNT; ++i) sem_destroy(&(shared->ingredients[i]));
				 sem_destroy(&(shared->working));
				 sem_destroy(&(shared->done));
				 shm_unlink(SHARED_MEMORY_NAME);
				 return EXIT_FAILURE;
		default: childList[++currentChild] = status;
	}
	
	/*  To assume there is exactly one chef that can cook the sekerpare there must
		be 6 chefs (dual combinations of ingredient count) with different infinite ingredients */
	for (i=0; i<INGREDIENT_COUNT; ++i) {
		for (j=0; j<INGREDIENT_COUNT; ++j) {
			if (j > i) {
				status = fork();
				switch(status) {
					case -1: fprintf(stderr, "\nSystem Error\nFork failed for chef #%d\n%s\n", currentChild, strerror(errno));
							 for(i=0; i<currentChild; ++i) kill(childList[i], SIGINT);
							 for (i=0; i<INGREDIENT_COUNT; ++i) sem_destroy(&(shared->ingredients[i]));
							 sem_destroy(&(shared->working));
							 sem_destroy(&(shared->done));
							 shm_unlink(SHARED_MEMORY_NAME);
							 return EXIT_FAILURE;
					case 0 : Chef(currentChild++, sharedDescriptor, i, j);
					default: childList[currentChild++] = status;
				}
			}
		}
	}
	
	
	/* Waiting for all childs until they are dead, reentering if wait interrupted by signal */
	for (i=0; i<currentChild; ++i) {
		while (wait(&childList[i]) == ERROR_CODE && errno == EINTR);
	}
	
	
	/* Destroying semaphores, unlinking shared memory and returning gracefully to OS */
	for (i=0; i<INGREDIENT_COUNT; ++i) sem_destroy(&(shared->ingredients[i]));
	sem_destroy(&(shared->working));
	sem_destroy(&(shared->done));
	shm_unlink(SHARED_MEMORY_NAME);
	return EXIT_SUCCESS;
}



/* Signal handler function raise the flag that indicates the end of work */
void handler(int signo) {
	keepWorking = FALSE;
}



/* Returns the name of the ingredient according to the index number */
char *GetName(Ingredient ingredient) {
	return (ingredient == Milk ? "Milk" : (ingredient == Sugar ? "Sugar" : (ingredient == Flour ? "Flour" : "Butter")));
}



/* Function for chefs to get their missing ingredients as parameters and wait for them
 * to be supplied by wholesaler to bake the şekerpare until SIGINT signal recieved */
void Chef(int id, int descriptor, Ingredient firstRequired, Ingredient secondRequired) {
	SharedStructure *shared = NULL;
	
	
	/* Getting shared structure to communicate with other processes */
	shared = (SharedStructure*)mmap(NULL, sizeof(SharedStructure), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, ZERO);
	
	/* Iterating until SIGINT recieved */
	while (keepWorking) {
		/* Acquire the lock to process ingredients */
		sem_wait(&shared->working);
		
		
		/* First ingredient couldn't acquired, try again with next ingredient set */
		if (sem_trywait(&shared->ingredients[firstRequired]) && errno == EAGAIN) {
			/* Intentionally left blank (rather then using continue) to release the processing lock */
		}
		
		/* First ingredient acquired but second couldn't. So release the first lock to
		 * let other chefs bake and try again with next ingredient set */
		else if (sem_trywait(&shared->ingredients[secondRequired]) && errno == EAGAIN) {
			sem_post(&shared->ingredients[firstRequired]);
		}
		
		/* Both locks for ingredients succesfully acquired, notify wholesaler that şekerpare is baked */
		else {
			fprintf(stderr, "Chef #%d baked the şekerpare with supplied %s and %s\n\n", id+1, GetName(firstRequired), GetName(secondRequired));
			sem_post(&shared->done);
		}
		
		
		/* Release the processing lock */
		sem_post(&shared->working);
	}

	printf("Chef %d exit\n", id+1);
	_exit(EXIT_SUCCESS);
}



/* Function for wholesaler to produce 2 random distinct ingredients for chefs
 * and wait for one of them to bake the şekerpare until SIGINT signal recieved */
void WholeSaler(int descriptor) {
	SharedStructure *shared = NULL;
	int first, second;
	
	
	/* Getting shared structure to communicate with other processes */
	shared = (SharedStructure*)mmap(NULL, sizeof(SharedStructure), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, ZERO);
	
	
	/* Iterating untill SIGINT recieved */
	while (keepWorking) {
		/* Produces 2 random ingredients different than each other */
		first = (random() % INGREDIENT_COUNT);
		while ((second = random() % INGREDIENT_COUNT) == first);
		
		
		/* Acquire the lock to supply ingredients */
		sem_wait(&shared->working);
		fprintf(stderr, "Wholesaler is dropping %s and %s\n", GetName(first), GetName(second));
		
		/* Increase the number of produced ingredients */
		sem_post(&shared->ingredients[first]);
		sem_post(&shared->ingredients[second]);
		
		/* Release the process lock and wait for şekerpare to be baked */
		sem_post(&shared->working);
		
		
		/* Checking flag in case no chefs remained to post the done semaphore */
		if (keepWorking) {
			fprintf(stderr, "Wholesaler is waiting for şekerpare\n\n");
			sem_wait(&shared->done);
		}
	}
	
	
	fprintf(stderr, "Saler exit\n");
	_exit(EXIT_SUCCESS);
}