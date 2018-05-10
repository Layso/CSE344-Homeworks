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
#include <time.h>
#include <semaphore.h>



/* Constant definitions */
#define ZERO 0
#define TRUE 1
#define FALSE 0
#define ERROR_CODE -1
#define LOCKED 0
#define UNLOCKED 1
#define FILE_PERMISSIONS S_IRUSR | S_IWUSR
#define SHARED_MEMORY_NAME "/151044001shm"
#define INGREDIENT_COUNT 4
#define CHEF_COUNT 6



/* Structure for Ingredients */
typedef enum {
	Milk,
	Sugar,
	Flour,
	Butter
} Ingredient;

/* Structure for shared memory */
typedef struct {
	sem_t done;
	sem_t working;
	sem_t ingredients[INGREDIENT_COUNT];
} SharedStructure;



/* Fuction prototypes */
void handler(int signo);
char *GetName(Ingredient ingredient);
void Chef(int id, int descriptor, Ingredient firstRequired, Ingredient secondRequired);
void WholeSaler(int descriptor);