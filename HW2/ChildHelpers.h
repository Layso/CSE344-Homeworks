#ifndef CHILD_HELPERS_H
#define CHILD_HELPERS_H
#define _POSIX_SOURCE

#include "Constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>



/* Function definitions */
void ReadSequence(int fileDescriptor, int n, double **sequence, int line);
int CalculateDFT(int n, double *sequence);
void ChildLogger(int line, int n, double *sequence, double dft);

#endif