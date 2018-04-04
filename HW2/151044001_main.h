#ifndef MAIN_H
#define MAIN_H
#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "Constants.h"



void ParentFunction(char fileName[], int maximum, int numberCount, pid_t childPID);
void ChildFunction(char fileName[], int numberCount);
int CountSequence(char fileName[], int itemCount, int itemSize);



#endif