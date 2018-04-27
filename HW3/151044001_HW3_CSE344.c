#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "151044001_HW3_CSE344.h"







int main(int argc, char *argv[]) {
	int i;
	char *command = NULL;
	char **commandHistory = NULL;
	char **commandList = NULL;
	
	
	/* Initializing string arrays */
	commandHistory = malloc(sizeof(char *));
	commandList = malloc(sizeof(char *));
	commandHistory[0] = NULL;
	commandList[0] = NULL;
	
	
	/* Infinitely taking commands until the exit command */
	do {
		ReadLine(&command);
		if (command != NULL) {
			SplitCommand(command, &commandList);
			AddToHistory(command, &commandHistory);
			
			for (i=0; commandList[i]!=NULL; ++i)
				free(commandList[i]);
			free(commandList);
			free(command);
		}
	} while(command != NULL);
	
	return EXIT_SUCCESS;
}



/* Function to create command list from  */
void SplitCommand(char *command, char ***list) {
	int i;
	int size = ZERO;
	int length = strlen(command);
	char *token;
	
	
	/* Allocating memory for command list */
	*list = malloc(sizeof(char*));
	
	/* Getting tokens from command to fill the array */
	token = strtok(command, " ");
	do {
		(*list)[size] = malloc(strlen(token));
		strcpy((*list)[size++], token);
		*list = realloc(*list, (size+1)*sizeof(char*));
		(*list)[size] = NULL;
	} while ((token = strtok(NULL, " ")) != NULL);
	
	
	/* Recrateing the messed command string */
	memset(command, ZERO, length);
	for (i=0; i<size; ++i) {
		command = strcat(command, (*list)[i]);
		
		if (i+1 != size+1) {
			command = strcat(command, " ");
		}
		
		else {
			command = strcat(command, "\0");
		}
	}
}



/* Function to read input without having any string size */
void ReadLine(char **command) {
	int continueRead = TRUE;
	int length = ZERO;
	int bytesRead;
	
	
	/* Allocationg initial space for command */
	if ((*command = malloc(CHAR_LENGTH_INTERVAL * sizeof(char))) == NULL) {
		fprintf(stderr, "\nError\nMemory allocation to read input failed\n");
		*command = NULL;
	}
	
	else {
		do {
			/* Read from stdin to command variable */
			bytesRead = read(STDIN_FILENO, (*command)+length, CHAR_LENGTH_INTERVAL);
			if (bytesRead == ERROR_CODE) {
				fprintf(stderr, "\nError\nMemory allocation to read input failed\n");
				free(*command);
				*command = NULL;
			}
			
			else {
				length += bytesRead;
				*command = realloc(*command, length+CHAR_LENGTH_INTERVAL);
				
				/* If new line character read, end the input taking process */
				if ((*command)[length-1] == CHAR_NEW_LINE) {
					(*command)[length-1] = CHAR_NULL;
					continueRead = FALSE;
				}
			}
		} while(continueRead);
	}
}



/* Helper function to check if given command is a valid one */
int ValidateCommand(char *command) {
	return !strcmp(COMMAND_CAT, command) || !strcmp(COMMAND_CD, command) || !strcmp(COMMAND_EXIT, command) || !strcmp(COMMAND_HELP, command) || !strcmp(COMMAND_LS, command) || !strcmp(COMMAND_PWD, command) || !strcmp(COMMAND_WC, command);
}



/* Function to insert new element to history array and resize it */
void AddToHistory(char *command, char ***history) {
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