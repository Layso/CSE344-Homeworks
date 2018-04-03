#ifndef MAIN_H
#define MAIN_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "Constants.h"



void ParentFunction(char fileName[], int maximum, int numberCount, pid_t childPID);
void ChildFunction(char fileName[], int numberCount);



#endif