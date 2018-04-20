#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <sys/stat.h>
//Function to make our arg array
void makeArgArray(struct parse * x )
{
	int bigArgSize=0;
	int j;
	int i=0;
	int index=0;

	//creating the argument arrays used for exec
	for(j=0;j<x->commSize;j++)
	{
		
		//create array for each number of commands
		//char * argArr[100];
		char** argArr;
		argArr = (char **)malloc (100 * sizeof (char *));
		int f;
		for(f=0;f<100;f++)
			argArr[f] = (char *)malloc (8 * sizeof (char));
		//printf("addr: %d\n",argArr );
		for(;i<x->argSize;i++)
		{
			//printf("here\n");
			argArr[index]=x->args[i];
			
			//printf("argArr[i] %s\n", x->args[i]);
			if(x->args[i]==NULL)
			{			
				i++;
				//do work before we break
				x->BigArgs[j]=argArr; //should revisit this, i casted it to triple pointer but not sure if this is correct solution
				bigArgSize++;
				index=0;
				break;
			}
			index++;
		}			
	}
		x->BigArgs[bigArgSize]=NULL; //set last element to NULL since these are the commands we will loop thrtough
}

//create multiple pipes or just one pipe
void execPipe(char ***cmd, struct parse * x) 
{
	int prog_status=0;
	int i,j,k;
	//printf("BigargArr[i] %s\n", cmd[0][0]);
	//printf("BigargArr[i] %s\n", cmd[1][0]);
	pid_t wpid;
  	int p[2];
  	int status;
  	pid_t pid;
  	int fd_in = 0;

    if(strcmp((*cmd)[0],"cd")==0) //if its a cd command we dont exec it
    {
        //printf("got here\n");
        int dir=chdir((*cmd)[1]);
        if(dir!=0)
        	perror("chdir error: ");
        return;

    }

  	while (*cmd != NULL)
    {
      	pipe(p);
      	if ((pid = fork()) == -1)
        {
          	exit(EXIT_FAILURE);
        }
      	else if (pid == 0)
        {
	        	//after for but before exec we do redirection
			if(x->input[1]!=NULL)// if theres an input redir
			{
				int fdIn=open(x->input[1], O_RDONLY);
				if(fdIn==-1)
				{
					perror("open error: ");
					return;
				}
				dup2(fdIn, 0);
				close(fdIn);
			}
			if(x->output[0]!=NULL)// if theres an output redir
			{
				int fdOut;
				if(strcmp(x->output[0],">>")==0)
				{
					//printf("heres\n");
					fdOut=open(x->output[1],O_WRONLY|O_CREAT | O_APPEND, 0666);
				}
				else
				{
					//printf("heres\n");
					fdOut=open(x->output[1],O_WRONLY|O_CREAT | O_TRUNC,0666);
				}
				//int fdOut = open(output[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
				if(fdOut==-1)
				{
					perror("open error: ");
					return;
				}
				dup2(fdOut, 1);
				//printf("args: %d\n",args[0] );
				close(fdOut);
			}
          	dup2(fd_in, 0); //change the input according to the old one 
          	if (*(cmd + 1) != NULL)
            	dup2(p[1], 1);
          	close(p[0]);

          	if(execvp((*cmd)[0], *cmd) ==-1)
          		perror("execvp error: ");
          	exit(EXIT_FAILURE);
        }
      	else
        {
          	wait(&prog_status);
          	close(p[1]);
          	fd_in = p[0]; //save the input for the next command
          	cmd++;
        }
    }
}
int execScript(char *argv[], struct parse * x)
{
	char * mybuf;
	char * statbuf = malloc(sizeof(struct stat));
	struct stat mystat;
	//char str[] ="/home/cabanae/Parse/ms";
	//get the working directory of a script we are trying to run
	mybuf=(char *)malloc(100);
	getcwd(mybuf,100);
	strcat(mybuf,"/");
	strcat(mybuf,argv[1]);
	//if the file doesnt exist in the same directory then we will let execvp search for file
	if(stat(mybuf,&mystat)!=0)
		strcpy(mybuf,argv[1]);
	//printf("mybuf: %s\n",mybuf );
	//i need to set the executable bit on to run script directly
	chmod(mybuf, S_IRWXU|S_IRGRP|S_IROTH);
	x=Parse(mybuf);
	makeArgArray(x);
	//printf("? ");
	execPipe((x->BigArgs),x);
	//setting file permissions to 644
	chmod(mybuf, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	free(mybuf);
	free(statbuf);
	return 0;		
}
void getArgs(struct parse *x, char*arguments[],char*arguments2[],char * currComm, int * argSize)
{
	typedef int bool;
	enum { false, true };
	bool isFound=false;
	bool isSet=false;
	bool isPipe=false;
	//int argSize=0;
	int argSize2=0;
	int j=0;
	for(;j<x->newLineSize;j++)
	{
		//if the command is found in newline
		if( strcmp(x->commands[0],x->newLine[j]) ==0 && isFound==false)
		{
			isFound=true;
			int k;		
				//k==j+1 because we found it at j, but next element in array is the arguments
				for(k=j+1;k<x->newLineSize;k++)
				{
					if(isPipe==true)
					{
						if(strcmp(x->newLine[k],">")!=0 && strcmp(x->newLine[k],">>")!=0 && strcmp(x->newLine[k],"<")!=0 && strcmp(x->newLine[k],"<<")!=0 &&isSet==false)
						{
							//printf("go cavs\n");
							arguments2[argSize2] = x->newLine[k]; //set the argument into the arguments array
						}
						else
						{
							isSet=true;
							continue;
						}
						isSet=false;
						argSize2++;
					}
					//if not a pipe
					if(strcmp(x->newLine[k],"|")!=0 && isPipe==false)
					{
						if((*argSize)==0)
						{
							//printf("got here fam\n");
							arguments[(*argSize)]=currComm;
							(*argSize)++;
						}
						if(strcmp(x->newLine[k],">")!=0 && strcmp(x->newLine[k],">>")!=0 && strcmp(x->newLine[k],"<")!=0 && strcmp(x->newLine[k],"<<")!=0 &&isSet==false)
						{
							//printf("go cavs\n");
							arguments[(*argSize)] = x->newLine[k]; //set the argument into the arguments array
						}
						else
						{
							isSet=true;
							continue;
						}
						isSet=false;
						(*argSize)++;
					}
					else
					{
						isPipe=true;
						isSet=false;
					}
				}					

					break;
		}
	}
	isFound=false;
	if((*argSize)==0)
	{
		arguments[0]=currComm;
		(*argSize)++;
	}
}
void clearArgs( char*arguments[],char*arguments2[])
{
	int s;	
	for(s=0;s<MAX;s++) //clearing
	{
		arguments[s]=NULL;
		arguments2[s]=NULL;
	}
}