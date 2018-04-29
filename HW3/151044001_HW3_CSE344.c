#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include <fcntl.h>
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
			AddToHistory(command, &commandHistory);
			SplitCommand(command, &commandList);
			if (ValidateCommandList(commandList)) {
				status = ExecuteCommands(commandList, commandHistory, command);
			}
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
int ExecuteCommands(char **list, char **history, char *command) {
	int token;
	int child;
	int i = ZERO;
	int fileDescriptor;
	int lastCommand = ZERO;
	int secondLastCommand;
	int redirectorRight = FALSE;
	int commandSize = ZERO;
	int pipeArr[PIPE_ARRAY_SIZE];
	
	
	/* Counting total command and pipe numbers */
	while (list[i] != NULL && !redirectorRight) {
		token = ValidateToken(list[i++]);
		if (token == Command) {
			++commandSize;
			
			secondLastCommand = lastCommand;
			lastCommand = i-1;
		} 
		
		if (list[i]!=NULL && !strcmp(list[i], DIRECTION_RIGHT)) redirectorRight = TRUE;
	}
	
	
	
	/* Checking for exit command */
	if (!strcmp(list[0], COMMAND_EXIT)) {
		return FALSE;
	}
	
	/* Checking for change directory command */
	else if (!strcmp(list[0], COMMAND_CD)) {
		if (list[2] != NULL) {
			printf("cd: can not resolve path: %s %s\n", list[1], list[2]);
		}
		
		else {
			CommandCd(list[1]);
		}
	}
	
	/* Checking for single commands */
	else if (commandSize == 1 || (commandSize > 2 && strcmp(list[lastCommand], COMMAND_WC))) {
		i = 0;
		child = fork();
		switch(child) {
			case 0: 	
				/* Output redirection process (if necessary) */
				if (redirectorRight) {
					while(list[i+1] != NULL) ++i;
					fileDescriptor = open(list[i], O_CREAT | O_WRONLY | O_EXCL, FILE_PERMISSIONS);
					if (fileDescriptor != ERROR_CODE) {
						dup2(fileDescriptor, STDOUT_FILENO);
						i = 0;
					}
					
					else {
						perror("redirector");
					}
				}
				
				/* Executable commands */
				if (i == 0 && IsSystemCommand(list[lastCommand])) {
					execve(list[lastCommand], list, NULL);
				}
				
				/* Built-in commands */
				else if (i == 0) {
					if (!strcmp(list[lastCommand], COMMAND_HELP)) {
						CommandHelp();
					}
					
					else if (!strcmp(list[lastCommand], COMMAND_PWD)) {
						CommandPwd();
					}
					
					else if (!strcmp(list[lastCommand], COMMAND_HISTORY)) {
						CommandHistory(list[lastCommand+1] == NULL ? 0 : atoi(list[lastCommand+1]), history);
					}
				}
				
				/* Deallocating the heap space before exit */
				for (i=0; history[i]!=NULL; ++i)
					free(history[i]);
				
				for (i=0; list[i]!=NULL; ++i)
					free(list[i]);
				free(command);
				free(history);
				free(list);
				_exit(EXIT_SUCCESS);
			case -1: fprintf(stderr, "\nSystem Error\nFork failed\n%s", strerror(errno)); break;
			default: wait(&child);
		}
	}
	
	/* Commands that require pipe which means last command is wc */
	else {
		if (pipe(pipeArr) == ERROR_CODE) {
			fprintf(stderr, "\nSystem Error\nFork failed\n%s", strerror(errno));
		}
		
		
		child = fork();
		switch (child) {
			case 0:
				i = 0;
				dup2(pipeArr[PIPE_WRITE], STDOUT_FILENO);
				
				
				/* If previous command is also wc then the result is certain */
				if (!strcmp(list[secondLastCommand], COMMAND_WC)) {
					printf("The commands from beginning to here doesn't change the result since the result of wc | wc will be equal to 1\n%c", '\0');
					fflush(stdout);
				}
				
				/* Executable commands */
				else if (IsSystemCommand(list[secondLastCommand])) {
					execve(list[secondLastCommand], list, NULL);
				}
				
				/* Built-in commands */
				else if (i == 0) {
					if (!strcmp(list[secondLastCommand], COMMAND_HELP)) {
						CommandHelp();
					}
					
					else if (!strcmp(list[secondLastCommand], COMMAND_PWD)) {
						CommandPwd();
					}
					
					else if (!strcmp(list[secondLastCommand], COMMAND_HISTORY)) {
						CommandHistory(list[secondLastCommand+1] == NULL ? 0 : atoi(list[secondLastCommand+1]), history);
					}
				}
				
				/* Deallocating the heap space before exit */
				for (i=0; history[i]!=NULL; ++i)
					free(history[i]);
				
				for (i=0; list[i]!=NULL; ++i)
					free(list[i]);
				free(command);
				free(history);
				free(list);
				_exit(EXIT_SUCCESS);
			case -1:  fprintf(stderr, "\nSystem Error\nFork failed\n%s", strerror(errno)); break;
			default: wait(&child);
		}
		
		child = fork();
		switch (child) {
			case 0:
				i = 0;
				dup2(pipeArr[PIPE_READ], STDIN_FILENO);
				
				/* Output redirection process (if necessary) */
				if (redirectorRight) {
					i = 0;
					while(list[i+1] != NULL) ++i;
					fileDescriptor = open(list[i], O_CREAT | O_WRONLY | O_EXCL, FILE_PERMISSIONS);
					if (fileDescriptor != ERROR_CODE) {
						dup2(fileDescriptor, STDOUT_FILENO);
						i = 0;
					}
					
					else {
						perror("redirector");
					}
				}
				
				/* Executable commands */
				if (i == 0 && IsSystemCommand(list[lastCommand])) {
					execve(list[lastCommand], list, NULL);
				}
				
				/* Built-in commands */
				else if (i == 0) {
					if (!strcmp(list[lastCommand], COMMAND_HELP)) {
						CommandHelp();
					}
					
					else if (!strcmp(list[lastCommand], COMMAND_PWD)) {
						CommandPwd();
					}
					
					else if (!strcmp(list[lastCommand], COMMAND_HISTORY)) {
						CommandHistory(list[lastCommand+1] == NULL ? 0 : atoi(list[lastCommand+1]), history);
					}
				}
				
				/* Deallocating the heap space before exit */
				for (i=0; history[i]!=NULL; ++i)
					free(history[i]);
				
				for (i=0; list[i]!=NULL; ++i)
					free(list[i]);
				free(command);
				free(history);
				free(list);
				_exit(EXIT_SUCCESS);
			case -1:  fprintf(stderr, "\nSystem Error\nFork failed\n%s", strerror(errno)); break;
			default: wait(&child);
		}
	}
	
	return TRUE;
}



/* Returns true if it is system command, returns false if it is built in command */
int IsSystemCommand(char *command) {
	if (!strcmp(command, COMMAND_LS) || !strcmp(command, COMMAND_CAT) || !strcmp(command, COMMAND_WC))
		return TRUE;
		
	else
		return FALSE;
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
			printf("error: conjunctions can't be at the beginning or at the end: %s\n", commandList[i]);
			return FALSE;
		}
		
		/* Redirectors must be between a command and a file name that is the last token */
		else if (currentToken == Redirector && nextToken != -1 && commandList[i+2] != NULL) {
			printf("error: redirectors must be between last command and a file name: %s %s %s\n", commandList[i-1], commandList[i], commandList[i+1]);
			return FALSE;
		}
		
		/* There must be valid commands around pipes */
		else if (currentToken == Pipe && (nextToken != Command)) {
			printf("error: pipes must be used between valid commands: %s %s %s\n", commandList[i-1], commandList[i], commandList[i+1]);
			return FALSE;
		}
		
		/* A command can not be fed by both file and pipe */
		else if (currentToken == Pipe && commandList[i+2] != NULL && !strcmp(commandList[i+2], DIRECTION_LEFT)) {
			printf("error: commands can't be fed by both files and pipes: %s %s %s\n", commandList[i], commandList[i+1], commandList[i+2]);
			return FALSE;
		}
		
		/* Command specific checks */
		else if (currentToken == Command) {
			/* There must be pipe between commands */
			if (nextToken == Command) {
				printf("error: commands can't be used sequentially without pipe: %s %s\n", commandList[i], commandList[i+1]);
				return FALSE;
			}
			
			/* Exit command must be alone */
			else if (!strcmp(commandList[i], COMMAND_EXIT) && (previousToken != -1 || nextToken != -1)) {
				printf("error: exit command must be alone\n");
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
}



/* Built in cd command */
void CommandCd(char *path) {
	if (chdir(path) == ERROR_CODE) {
		perror("cd");
	}
	
	printf("%c", '\0');
	fflush(STDIN_FILENO);
}



/* Built in help command */
void CommandHelp() {
	printf("HW3 - CSE344 - System Programming - Gebze Technical University\nThis is a simple shell implementation for the homework of system programming course\nFeel free to use this program in any ways since there isn't any license\nImplemented by Deniz Can Erdem Yılmaz, spring 2018\n\n\n");
	
	printf("Supported commands:\n  ls              ->  Prints information about files on current directory\n  pwd             ->  Prints current working directory\n  cd [path]       ->  Changes current working directory with given (supports both normal and absolute) path\n");
	
	printf("  help            ->  Prints information about shell\n  cat [filename]  ->  Prints the content of given file\n  wc [filename]   ->  Counts the lines of given file\n  history [n]     ->  Prints the nth command on history, prints whole history if n doesn't provided\n  exit            ->  Shuts the shell down\n\n\n");
	
	printf("Supported conjunctors:\n  |  ->  Can connect the output of a command to input of another command\n  <  ->  Can redirect the file content to command as input\n  >  ->  Can redirect the output of command to file\n");
	printf("%c", '\0');
	fflush(STDIN_FILENO);
}



/* Built in pwd command */
void CommandPwd() {
	char path[FILENAME_MAX];
	getcwd(path, FILENAME_MAX);
	printf("%s\n%c", path, '\0');
	fflush(STDIN_FILENO);
}



/* Built in history command */
void CommandHistory(int n, char**history) {
	int i = ZERO;
	
	while(history[i] != NULL) {
		if (n <= ZERO || n==i+1) {
			printf("%d- %s\n", i+1, history[i]);
		}
		
		++i;
	}
	printf("%c", '\0');
	fflush(STDIN_FILENO);
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
	
	else {
		(*list)[0] = NULL;
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
	char path[FILENAME_MAX];
	int i;
	int status = FALSE;
	
	
	/* Allocationg initial space for command */
	if ((*command = malloc(CHAR_LENGTH_INTERVAL * sizeof(char))) == NULL) {
		fprintf(stderr, "\nError\nMemory allocation to read input failed\n");
		*command = NULL;
	}
	
	else {
		printf("%s$ ", getcwd(path, FILENAME_MAX));
		fflush(stdout);
		
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
		if (strlen(*command) == ZERO) {
			free(*command);
			*command = NULL;
		}
		
		else {
			for (i=0; i<strlen(*command); ++i)
				if ((*command)[i] != ' ')
					status = TRUE;
			
			if (!status) {
				free(*command);
				*command = NULL;
			}
		}
	}
}



/* Helper function to check if given command is a valid one */
int ValidateToken(char *command) {
	if (!strcmp(COMMAND_CAT, command) || !strcmp(COMMAND_CD, command) || !strcmp(COMMAND_EXIT, command) || !strcmp(COMMAND_HELP, command) || !strcmp(COMMAND_LS, command) || !strcmp(COMMAND_PWD, command) || !strcmp(COMMAND_WC, command) || !strcmp(COMMAND_HISTORY, command)) {
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