#ifndef MAIN_H
#define MAIN_H

#include <sys/types.h>



void ParentFunction(char fileName[], int maximum, int numberCount, pid_t childPID);
void ChildFunction(char fileName[], int numberCount);




#endif