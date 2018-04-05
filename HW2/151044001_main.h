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
#include <sys/wait.h>
#include <signal.h>
#include "Constants.h"



void ParentFunction(char fileName[], int maximum, int itemCount, pid_t childPID);
void ChildFunction(char fileName[], int maximum, int itemCount);
void EmptyHandler(int signal);
void ChildHandler(int signal);
int CountSequence(char fileName[], int itemCount, int itemSize);

#endif