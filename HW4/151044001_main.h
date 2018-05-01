#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <wait.h>
#include <errno.h>


#define ZERO 0
#define ERROR_CODE -1
#define FILE_PERMISSIONS S_IRUSR | S_IWUSR
#define SHARED_MEMORY_NAME "/151044001shm"