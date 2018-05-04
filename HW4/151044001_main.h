#define _POSIX_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <wait.h>
#include <errno.h>
#include <signal.h>



#define ZERO 0
#define TRUE 1
#define FALSE 0
#define ERROR_CODE -1
#define FILE_PERMISSIONS S_IRUSR | S_IWUSR
#define SHARED_MEMORY_NAME "/151044001shm"
#define REQUIRED_ARGC 2
#define EXE_NAME_INDEX 0
#define CHEF_COUNT_INDEX 1
#define INGREDIENT_COUNT 4
#define MINIMUM_CHEF_COUNT 6


typedef enum {
	Milk,
	Sugar,
	Flour,
	Butter
} Ingredient;



void handler(int signo);
char *GetName(Ingredient ingredient);
void Chef(int id, int descriptor, Ingredient firstIngredient, Ingredient secondIngredient);