#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "parse.h"
void makeArgArray(struct parse * x );
void getArgs(struct parse *x, char*arguments[],char*arguments2[],char * currComm, int * argSize);
void execPipe(char ***cmd, struct parse * x);
int execScript(char *argv[], struct parse * x);
void clearArgs( char*arguments[],char*arguments2[]);
#endif