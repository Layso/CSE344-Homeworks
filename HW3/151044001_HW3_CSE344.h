#ifndef MAIN_H
#define MAIN_H



/* Constant definitions */
#define ZERO 0
#define TRUE 1
#define FALSE 0
#define ERROR_CODE -1
#define CHAR_LENGTH_INTERVAL 10
#define CHAR_NEW_LINE '\n'
#define CHAR_NULL '\0'
#define CHAR_BLANK ' '
#define COMMAND_LS "ls"
#define COMMAND_PWD "pwd"
#define COMMAND_CD "cd"
#define COMMAND_HELP "help"
#define COMMAND_CAT "cat"
#define COMMAND_WC "wc"
#define COMMAND_EXIT "exit"
#define DIRECTION_LEFT "<"
#define DIRECTION_RIGHT ">"
#define PIPELINE "|"



/* Function prototypes */
void ReadLine(char **command);
int ValidateCommand(char *command);
void AddToHistory(char *command, char ***history);
void SplitCommand(char *command, char ***list);
#endif