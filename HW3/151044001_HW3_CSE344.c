#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "151044001_HW3_CSE344.h"







int main(int argc, char *argv[]) {
	char *command = NULL;
	char **commandHistory = NULL;
	int i;
	
	/* Initializing command history array */
	commandHistory = malloc(sizeof(char *));
	commandHistory[0] = NULL;
	
	
	/* Infinitely taking commands until the exit command */
	do {
		command = ReadLine();
		if (IsValid(command)) {
			AddToHistory(&commandHistory, command);
			
			/*printf("\nCommand hist:\n");*/
			for (i=0; commandHistory[i]!=NULL; ++i) {
				printf("%d) %s\n", i, commandHistory[i]);
			}
			/*printf("Read commmand: %s\n", command);*/
		}
		
		else {
			fprintf(stderr, "%s: command not found\n", command);
		}
		
		free(command);
		/*lastCommand = NULL;*/	
	} while(command != NULL);
	
	
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
		return NULL;
	}
	
	else {
		do {
			/* Read from stdin to command variable */
			bytesRead = read(STDIN_FILENO, command+length, CHAR_LENGTH_INTERVAL);
			if (bytesRead == ERROR_CODE) {
				fprintf(stderr, "\nError\nMemory allocation to read input failed\n");
				free(command);
				return NULL;
			}
			
			else {
				length += bytesRead;
				command = realloc(command, length+CHAR_LENGTH_INTERVAL);
				
				/* If new line character read, end the input taking process */
				if (command[length-1] == CHAR_NEW_LINE) {
					command[length-1] = CHAR_NULL;
					continueRead = FALSE;
				}
			}
		} while(continueRead);
	}
	
	
	return command;
}



/* Helper function to check if given command is a valid one */
int IsValid(char *command) {
	return !strcmp(COMMAND_CAT, command) || !strcmp(COMMAND_CD, command) || !strcmp(COMMAND_EXIT, command) || !strcmp(COMMAND_HELP, command) || !strcmp(COMMAND_LS, command) || !strcmp(COMMAND_PWD, command) || !strcmp(COMMAND_WC, command);
}



/* Function to insert new element to history array and resize it */
void AddToHistory(char ***history, char *command) {
	int i;
	int size = ZERO;
	char **newArray = NULL;
	
	
	/* Counting old array size */
	while((*history)[size++] != NULL);
	
	
	/* Comparing last command with new command to prevent piling commands on history */
	if (size == 1 || strcmp((*history)[size-2], command)) {
		/* Creating new array with new size and all elements */
		newArray = malloc((size+1) * sizeof(char*));
		for (i=0; i<size-1; ++i) {
			newArray[i] = malloc(strlen((*history)[i]) * sizeof(char));
			strcpy(newArray[i], (*history)[i]);
			free((*history)[i]);
		}
		
		/* Adding new command to end of history */
		newArray[i] = malloc(strlen(command));
		strcpy(newArray[i], command);
		
		/* Deallocating old array completely to assign new array to it */
		newArray[++i] = NULL;
		free(*history);
		*history = newArray;
	}
}