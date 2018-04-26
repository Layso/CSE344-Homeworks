#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "151044001_HW3_CSE344.h"







int main(int argc, char *argv[]) {
	char *lastCommand = NULL;
	
	
	do {
		lastCommand = ReadLine();
		
		printf("Read commmand: %s\n", lastCommand);
		free(lastCommand);
		lastCommand = NULL;	
	} while(lastCommand != NULL);
	
	
	return EXIT_SUCCESS;
}



/* Function to read input without having any string size */
char *ReadLine() {
	char *command = NULL;
	int continueRead = TRUE;
	int length = ZERO;
	int bytesRead;
	
	
	/* Allocationg initial space for command */
	if ((command = malloc(CHAR_LENGTH_INTERVAL * sizeof(char))) == NULL) {
		fprintf(stderr, "\nError\nMemory allocation to read input failed\n");
		exit(EXIT_FAILURE);
	}
	
	else {
		do {
			/* Read from stdin to command variable */
			bytesRead = read(STDIN_FILENO, command+length, CHAR_LENGTH_INTERVAL);length += bytesRead;
			command = realloc(command, length+CHAR_LENGTH_INTERVAL);
			
			/* If new line character read, end the input taking process */
			if (command[length-1] == CHAR_NEW_LINE) {
				command[length-1] = CHAR_NULL;
				continueRead = FALSE;
			}
		} while(continueRead);
	}
	
	
	return command;
}