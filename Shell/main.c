/*
	Author: Evan Cabana
	Date: 5/22/2017
	Description: Command line for unix systems. Executes multiple pipes, scripts, options.
*/
#include <stdlib.h>
#include "functions.h"

int main(int argc,char *argv[])
{
	int status=0;
	char *buf;
	char ** ret;
	size_t read;
	size_t numBytes=0;
	struct parse *x;
	char * arguments[MAX]={NULL};
	char * arguments2[MAX]={NULL};
	char * currComm;
	int argSize=0;
	int *p;
	p=&argSize;
	//if theres a script from command line
	if(argc>1)
		execScript(argv,x);
	printf("? ");


	//main loop
	while(1)
	{

		buf=(char *)malloc(numBytes+1);
		read  = getline(&buf,&numBytes,stdin); //read input from stdin	
		if(read==-1) //if we want to exit
			return 1;
		else
			x = Parse(buf); //return a struct to print
		
		if(x->parsedLine[0][0]=='#') //ignore comments
		{
			printf("? ");
			continue;			
		}

		//loop through commands, for first command find in newLine array,
		//once found then find its arguments, which is everything after it up until we reach a pipe, or its end of the size of array	
		currComm=x->commands[0];	
		if(strcmp(currComm,"exit")==0)//exit
		{
			free(buf);
			exit(status);
		}
		getArgs(x,arguments,arguments2,currComm,p); //get the args		 	
		makeArgArray(x); //create arg array to execute commands
		printf("? ");
		execPipe((x->BigArgs),x); //where we execute our commands								
		argSize=0; 
		free(x); // freeing the memory allocated for the object returned from Parse
		clearArgs(arguments,arguments2); //clear args for next loop
	}
	free(buf);
	return 0;	
}
