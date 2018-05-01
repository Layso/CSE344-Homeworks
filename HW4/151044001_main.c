#include "151044001_main.h"


void parent(int fd) {
	int *memory;
	int i = ZERO;
	
	
	printf("i is: %d\n", i);
	memory = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, ZERO);
	sleep(2);
	i = *memory;
	
	printf("i is: %d\n", i);
}

void child(int fd) {
	int *memory;
	int i = 2;
	
	memory = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, ZERO);
	printf("mem is: %d\n", *memory);
	*memory = i;
	printf("mem is: %d\n", *memory);
	
	_exit(EXIT_SUCCESS);
}


int main(int argc, char **argv) {
	int sharedDescriptor;
	
	
	/* Creating descriptor with the desired shared memory name */
	sharedDescriptor = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, FILE_PERMISSIONS);
	if (sharedDescriptor == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nShared memory creation failed\n%s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	
	/* Changing size of file to the size of semaphores */
	if (ftruncate(sharedDescriptor, sizeof(int)) == ERROR_CODE) {
		fprintf(stderr, "\nSystem Error!\nMemory size extension failed\n%s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	
	
	switch(fork()) {
		case 0:	child(sharedDescriptor); break;
		default: parent(sharedDescriptor); wait(0); break;
	}
	
	
	shm_unlink(SHARED_MEMORY_NAME);
	return EXIT_SUCCESS;
}