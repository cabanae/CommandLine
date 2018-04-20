/*
	Author: Evan Cabana
	Date: 5/22/2017
	Description: Parse command line parse.c that parses the command line arguments
*/
#include "parse.h"
#include <string.h>
#include <stdlib.h>

#define MAXSMALL 100
//bool struct to help us
typedef int bool;
enum { false, true };
//Function to remove spaces
void RemoveSpaces(char* source)
{
	char* i = source;
  	char* j = source;
  	while(*j != 0)
  	{
    	*i = *j++;
    	if(*i != ' ')
      		i++;
  	}
  	*i = 0;
}
//parse function
struct parse * Parse(char *line)
{

	const char space[2] = " ";
	char *token;
	char *iarrow="<\n"; //adding newline for it to be equal for getline()
	char *oarrow=">\n"; //adding newline for it to be equal for getline()
	//get first token
	token = strtok(line,space);
	struct parse *x = malloc(sizeof *x); 

	x->commSize=0;
	x->totalPipes=0;
	x->parsedSize=0;
	x->argSize=0;
	
	//x->myArgCount=0;
	if(strlen(line)<=1) //empty string
	{
		return x; // return empty struct

	}
	int i;
	int k;
	bool commandSet=false;
	char *temp[MAX];

	//loop through all tokens
	while(token != NULL)
	{
		i=x->parsedSize;
		x->parsedLine[i]=token;
		x->parsedSize++;
		token= strtok(NULL,space);
	}
	
	int len =  strlen(x->parsedLine[i]);
	//need to get rid of new line character added to the end of the string
	if( x->parsedLine[i][len-1] == '\n' )
    	x->parsedLine[i][len-1] = 0;
	//char * args[MAXSMALL];

	for(k=0;k<x->parsedSize;k++)
	{
		//if we find the < string
		if(x->parsedLine[k]!=NULL)
		{
			//ignore #
			/*if(x->parsedLine[k][0]=='#') 
			{
				//printf("got here\n");
				continue;
			}*/
			if(strcmp(x->parsedLine[k],iarrow)==0 || strcmp(x->parsedLine[k],"<")==0 ||strcmp(x->parsedLine[k],"<<")==0  )
			{
				x->input[0] = x->parsedLine[k];
				x->input[1] = x->parsedLine[k+1];
				//set the old values to null
				x->parsedLine[k]=NULL;
				x->parsedLine[k+1]=NULL;
			}
			else if(strcmp(x->parsedLine[k],oarrow)==0 || strcmp(x->parsedLine[k],">")==0 || strcmp(x->parsedLine[k],">>")==0 )
			{
				x->output[0] = x->parsedLine[k]; //set where we found the character to the first slot
				x->output[1] = x->parsedLine[k+1];//set where we found the character to the second slot
				//set the old values to null
				x->parsedLine[k]=NULL;
				x->parsedLine[k+1]=NULL;
			}
			else if(strcmp(x->parsedLine[k],"|")!=0 && commandSet==false) //not equal then its a command
			{

				x->commands[(x->commSize)] = x->parsedLine[k];
				x->newLine[x->newLineSize] = x->parsedLine[k];
				x->args[x->argSize] = x->parsedLine[k]; //add it twice
				x->argSize++;




				//printf("parseline here: %s newline slot: %d\n",x->parsedLine[k],x->newLineSize);
				x->newLineSize++;
				x->commSize++;
				commandSet=true; //set this so next command isnt considered a 
				continue;
			}
			else if(strcmp(x->parsedLine[k],"|")==0)
			{
				x->newLine[x->newLineSize] = x->parsedLine[k];
				x->newLineSize++;
				x->totalPipes++;
				x->args[x->argSize] = NULL;
				x->argSize++;

			}
			else if(x->parsedLine[k][0]=='\\') // if it is a backwards slash
			{
				int countSlash=0;
				char * temp[MAX];
				int tempCount=0;
				bool slashSet=false;
				//remove first slash
				//memmove(x->parsedLine[k], x->parsedLine[k]+1, strlen(x->parsedLine[k]));
				//search entire string for backslash's that escape
				int g;
				//printf("size: %d\n",x->argSize );
				for(g=0;g<strlen(x->parsedLine[k]);g++)
				{
					if(x->parsedLine[k][g]=='\\' && slashSet==false)
					{	
						slashSet=true;
						countSlash++;
						//printf("slash: %d\n",countSlash );
						x->parsedLine[k][g] = ' ';
					}
					else
					{
						slashSet=false;
					}

				}
				RemoveSpaces(x->parsedLine[k]);
				x->newLine[x->newLineSize] = x->parsedLine[k];
				x->newLineSize++;
				x->args[x->argSize] = x->parsedLine[k];
				x->argSize++;
				//printf("parsedlined: %s\n", x->parsedLine[k]);
			}
			else if(x->parsedLine[k][0]=='-') //else if the first chracter in the string is an option skip and continue at top of loop
			{
				x->newLine[x->newLineSize] = x->parsedLine[k];
				x->newLineSize++;
				x->args[x->argSize] = x->parsedLine[k];
				x->argSize++;
				continue;
			}
			else // arguments for a command??
			{
				x->newLine[x->newLineSize] = x->parsedLine[k];
				x->newLineSize++;
				x->args[x->argSize] = x->parsedLine[k];
				x->argSize++;

			}

			commandSet=false;
		}
	}
	x->args[x->argSize] = NULL; //add the final null terminating string
	x->argSize++;

	//if we had input redirection add it to front of array, copy original array
	if(x->input[0]!=NULL)
	{
		//now copy every thing over two slots over and add new stuff at begining
		int f;
		int d=0;
		for(f=0;f<MAX;f++)
		{
			temp[f]=x->newLine[f];
			//printf("x->newline: %s \n",x->newLine[f]);
		}
		for(f=2;f<MAX;f++)
		{
			x->newLine[f]=temp[d];
			d++;
		}
		x->newLine[0]=x->input[0];
		x->newLine[1]=x->input[1];
		x->newLineSize+=2;
	}

	//if we had output redirection add it to end of array
	if(x->output[0]!=NULL)
	{
		//now copy every thing over two slots over and add new stuff at begining
		int f;
		int d=0;
		if(x->newLineSize<MAX) // less than our array, then append to the end
		{
			x->newLine[x->newLineSize]=x->output[0];
			x->newLine[x->newLineSize+1]=x->output[1];
			x->newLineSize+=2;
		}
	}

	//printLine(x); // print array
	return x;
	//free(x); // free the memory we're done

}

//function to print
void printLine(struct parse *x)
{
	//print number of commands
	//printf("%d: ",x->commSize);
	int num=0;
	int j;

	//print the new command line
	for(j=0;j<x->newLineSize;j++)
	{
		if(x->newLine[j]!=NULL) //if not null
		{
			if(strcmp(x->newLine[j],">") !=0 && strcmp(x->newLine[j],"<") !=0 && strcmp(x->newLine[j],"|") !=0 && strcmp(x->newLine[j],">>") !=0 && strcmp(x->newLine[j],"<<") !=0)
				printf("\'%s\' ",x->newLine[j]);
			else if(strcmp(x->newLine[j],"|") ==0)
				printf("%s ",x->newLine[j]);
			else
				printf("%s",x->newLine[j]);
		}
	}
	printf("\n");
}