#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include "151044001_HW3_CSE344.h"







int main(int argc, char *argv[]) {
	int i, status = TRUE;
	char *command = NULL;
	char **commandHistory = NULL;
	char **commandList = NULL;
	
	
	/* Initializing string arrays */
	commandHistory = malloc(sizeof(char *));
	commandHistory[0] = NULL;
	commandList = NULL;
	
	
	/* Infinitely taking commands until the exit command */
	do {
		ReadLine(&command);
		if (command != NULL) {
			SplitCommand(command, &commandList);
			if (ValidateCommandList(commandList)) {
				/*status = ExecuteCommands(commandList, &commandHistory);*/
			}
			
			else {
			}
			
			printf("\n");
			
			/* Clearing allocated memory for next command */
			for (i=0; commandList[i]!=NULL; ++i)
				free(commandList[i]);
			free(commandList);
			free(command);
		}
	} while(status);
	
	for (i=0; commandHistory[i]!=NULL; ++i)
		free(commandHistory[i]);
	free(commandHistory);
	
	return EXIT_SUCCESS;
}



/*  */
int ExecuteCommands(char **list, char ***history) {
	int size = -1;
	int pipes = ZERO;
	int i = ZERO;
	
	
	
	/* Counting list size and pipe count */
	while (list[size++] != NULL)
		if (!strcmp(list[size], PIPELINE))
			++pipes;
	
	
	do {
		
	} while(list[i] != NULL);
	
	return TRUE;
}



/*  */
char *GetExecutableNameByCommand(char *command) {
	return NULL;
}



/* Function to validate the order of the commands and conjunction characters */
int ValidateCommandList(char **commandList) {
	int i = -1;
	int currentToken;
	int previousToken;
	int nextToken;
	
	/* Contorlling each token with a loop */
	while (commandList[++i] != NULL) {
		currentToken = ValidateToken(commandList[i]);
		nextToken = commandList[i+1]!=NULL ? ValidateToken(commandList[i+1]) : -1;
		previousToken = i-1 >= ZERO ? ValidateToken(commandList[i-1]) : -1;
		
		
		/* There can't be conjunction character neither at the beginning nor at the end */
		if ((i == 0 || commandList[i+1] == NULL) && (currentToken == Pipe || currentToken == Redirector)) {
			printf("error: conjunctions can't be at the beginning or at the end\n");
			return FALSE;
		}
		
		/* Redirectors must be between a command and a file name that is the last token */
		else if (currentToken == Redirector && nextToken != -1 && commandList[i+2] != NULL) {
			printf("error: redirectors must be between last command and a file name\n");
			return FALSE;
		}
		
		/* There must be valid commands around pipes */
		else if (currentToken == Pipe && (nextToken != Command || previousToken != Command)) {
			printf("error: pipes must be used between valid commands: %s %s %s\n", commandList[i-1], commandList[i], commandList[i+1]);
			return FALSE;
		}
		
		else if (currentToken == Command) {
			/* There must be pipe between commands */
			if (nextToken == Command) {
				printf("error: commands can't be used sequentially without pipe: %s %s\n", commandList[i], commandList[i+1]);
				return FALSE;
			}
			
			/* Skipping possible arguments of command */
			else {
				while (commandList[i+1] != NULL && ValidateToken(commandList[i+1]) == Unknown) ++i;
			}
		}
		
		else if (currentToken == Unknown && (nextToken != -1 || previousToken == -1)) {
			printf("error: unknown command: %s\n", commandList[i]);
			return FALSE;
		}
	}
	
	
	return TRUE;	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/* If token is a conjunction */
	/* Conjunctions can't be at the beginning or the end */
	/* If the conjunction is a redirector */
	/* There must be a command near a redirector */
	/* There must be a file near a redirector */
	/* Pipes need both commands near them */
	/* If token is unknown (considered as file name) */
	/* If it is only token then it is considered an unknown command */
	/* There must be a conjunction near a file name */
	/* Files can not be pointed as output more than once to avoid overriding */
	/* If token is a command */
	/* Commands can not be sequential */
	/* Commands can not have multiple input sources */
	/*int i;
	int previousToken;
	int currentToken;
	int nextToken;
	
	
	for (i=0; commandList[i]!=NULL; ++i) {
		currentToken = ValidateToken(commandList[i]);
		nextToken = commandList[i+1]!=NULL ? ValidateToken(commandList[i+1]) : -1;
		previousToken = i-1 >= ZERO ? ValidateToken(commandList[i-1]) : -1;
		
		
		if (currentToken == Command) {
			if ((nextToken != -1 && nextToken == Command) || (previousToken!=-1 && previousToken==Command)) {
				printf("Conjunction characters needed between commands: %s %s %s\n", previousToken==-1 ? "" : commandList[i-1], commandList[i], nextToken==-1 ? "" : commandList[i+1]);
				return FALSE;
			}
			
			else if (previousToken == Pipe && (nextToken != -1 && !strcmp(DIRECTION_LEFT, commandList[i+1]))) {
				printf("Commands can not have multiple input sources: %s %s %s\n", previousToken==-1 ? "" : commandList[i-1], commandList[i], nextToken==-1 ? "" : commandList[i+1]);
				return FALSE;
			}
		}
		
		else if (currentToken == Redirector || currentToken == Pipe) {
			if (nextToken == -1 || previousToken == -1) {
				printf("Conjunction characters needed between commands and/or files: %s\n", commandList[i]);
				return FALSE;
			}
			
			else if (currentToken == Redirector) {
				if (nextToken != Command && previousToken != Command) {
					printf("Redirection characters needs a file name and a command: %s %s %s\n", commandList[i-1], commandList[i], commandList[i+1]);
					return FALSE;
				}
				
				else if (nextToken == Command && previousToken == Command) {
					printf("Redirection characters needs a file name and a command: %s %s %s\n", commandList[i-1], commandList[i], commandList[i+1]);
					return FALSE;
				}
			}
			
			else if (currentToken == Pipe && (nextToken != Command || previousToken != Command)) {
				printf("Pipe must be used between commands: %s %s %s\n", commandList[i-1], commandList[i], commandList[i+1]);
				return FALSE;
			}
		}
		
		else {
			if (previousToken == -1 && nextToken == -1) {
				printf("Unknown command: %s\n", commandList[i]);
				return FALSE;
			}
			
			else if ((previousToken != -1 && !strcmp(commandList[i-1], DIRECTION_LEFT) && !strcmp(commandList[i], DIRECTION_RIGHT)) && (nextToken != -1 && !strcmp(commandList[i+1], DIRECTION_LEFT) && !strcmp(commandList[i+1], DIRECTION_RIGHT))) {
				printf("File names need to be used with redirection characters: %s %s %s\n", commandList[i-1], commandList[i], commandList[i+1]);
				return FALSE;
			}
			
			else if (!strcmp(commandList[i-1], DIRECTION_RIGHT) && !strcmp(commandList[i+1], DIRECTION_LEFT)) {
				printf("Files can't be pointed multiple times as output destination to avoid overriding: %s %s %s\n", commandList[i-1], commandList[i], commandList[i+1]);
				return FALSE;
			}
		}
	}
	
	
	return TRUE;*/
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
	
	if (token != NULL) {
		do {
			(*list)[size] = malloc(strlen(token));
			strcpy((*list)[size++], token);
			*list = realloc(*list, (size+1)*sizeof(char*));
			(*list)[size] = NULL;
		} while ((token = strtok(NULL, " ")) != NULL);
	}
	
	
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
int ValidateToken(char *command) {
	if (!strcmp(COMMAND_CAT, command) || !strcmp(COMMAND_CD, command) || !strcmp(COMMAND_EXIT, command) || !strcmp(COMMAND_HELP, command) || !strcmp(COMMAND_LS, command) || !strcmp(COMMAND_PWD, command) || !strcmp(COMMAND_WC, command)) {
		return Command;
	}
	
	else if (!strcmp(DIRECTION_LEFT, command) || !strcmp(DIRECTION_RIGHT, command)) {
		return Redirector;
	}
	
	else if (!strcmp(PIPELINE, command)) {
		return Pipe;
	}
	
	else {
		return Unknown;
	}
}



/* Function to insert new element to history array and resize it */
void AddToHistory(char *command, char ***history) {
	int i;
	int size = ZERO;
	char **newArray = NULL;
	
	
	/* Counting old array size */
	while((*history)[size++] != NULL);
	
	
	/* Comparing last command with new command to prevent piling same command on history */
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