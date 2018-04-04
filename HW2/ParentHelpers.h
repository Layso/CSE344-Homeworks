#ifndef PARENT_HELPERS_H
#define PARENT_HELPERS_H

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Constants.h"


/* Function definitions */
void ProduceSequence(int n, double **sequence);
void WriteToFile(int fileDescriptor, int n, double *sequence, int maximumLines);


#endif