#include "151044001_main.h"



int keepWorking = TRUE;



int main(int argc, char **argv) {
	int i, j;
	int status;
	int totalChef;
	int *childList;
	int sharedDescriptor;
	int currentChef = ZERO;
	struct sigaction signalStruct;
	
	
	/* Controlling command line arguments */
	if (argc != REQUIRED_ARGC || (totalChef = atoi(argv[CHEF_COUNT_INDEX])) < MINIMUM_CHEF_COUNT) {
		fprintf(stderr, "Usage: %s [chefCount]\n", argv[EXE_NAME_INDEX]);
		fprintf(stderr, "       Chef count can be minimum %d \n", MINIMUM_CHEF_COUNT);
		return EXIT_FAILURE;
	}
	
	/* Creating signal handler to exit program gracefuly with SIGINT */
	memset(&signalStruct, ZERO, sizeof(struct sigaction));
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
	if (ftruncate(sharedDescriptor, sizeof(int)) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nMemory size extension failed\n%s\n", strerror(errno));
		shm_unlink(SHARED_MEMORY_NAME);
		return EXIT_FAILURE;
	}
	
	/* Creating child process id list for chefs */
	childList = malloc(totalChef * sizeof(int));
	
	
	/* To assume there is a chef who can prepare the sekerpare with the 2 random ingredients
	   that wholesaler provided, there must be at least 6 chefs with each dual combination
	   of 4 ingredients. We're making sure that we have each combination of ingredients */
	for (i=0; i<INGREDIENT_COUNT; ++i) {
		for (j=0; j<INGREDIENT_COUNT; ++j) {
			if (j > i) {
				status = fork();
				switch(status) {
					case 0 : Chef(++currentChef, sharedDescriptor, i, j); break;
					case -1: fprintf(stderr, "\nSystem Error\nFork failed for chef #%d\n%s\n", currentChef, strerror(errno));
							 for(i=0; i<currentChef; ++i) kill(childList[i], SIGINT);
							 i = INGREDIENT_COUNT;
							 j = INGREDIENT_COUNT;
							 break; 
					default: childList[++currentChef] = status;
				}
			}
		}
	}
	
	while(keepWorking);
	
	
	for (i=0; i<currentChef; ++i) {
		wait(childList[i]);
	}
	
	shm_unlink(SHARED_MEMORY_NAME);
	return EXIT_SUCCESS;
}



/* Signal handler function */
void handler(int signo) {
	keepWorking = FALSE;
}



/* Returns the name of the ingredient according to the index number */
char *GetName(Ingredient ingredient) {
	return (ingredient == Milk ? "Milk" : (ingredient == Sugar ? "Sugar" : (ingredient == Flour ? "Flour" : "Butter")));
}



/*  */
void Chef(int id, int descriptor, Ingredient firstIngredient, Ingredient secondIngredient) {
	printf("Chef #%d come alive with infinite %s and %s sources\n", id, GetName(firstIngredient), GetName(secondIngredient));
	while(keepWorking);
	_exit(EXIT_SUCCESS);
}