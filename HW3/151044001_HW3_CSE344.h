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
#define COMMAND_LS "ls"
#define COMMAND_PWD "pwd"
#define COMMAND_CD "cd"
#define COMMAND_HELP "help"
#define COMMAND_CAT "cat"
#define COMMAND_WC "wc"
#define COMMAND_EXIT "exit"



/*  */
char *ReadLine();
int IsValid(char *command);
void AddToHistory(char ***history, char *command);
#endif