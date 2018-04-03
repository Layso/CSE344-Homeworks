#ifndef PARENT_HELPERS_H
#define PARENT_HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Constants.h"


/* Function definitions */
void ProduceSequence(int n, double **sequence);
void WriteToFile(char fileName[], int n, double *sequence, int maximumLines);


#endif