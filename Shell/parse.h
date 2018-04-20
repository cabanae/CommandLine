/*
	Author: Evan Cabana
	ID: 46834927
	Date: 5/22/2017
	Description: Parse command line Parse.h
*/
#ifndef PARSE_H
#define PARSE_H
#include <stdio.h>
#define MAX 10000 // max limit for our array
//struct for parse
struct parse{
	char *commands[MAX];	//store the commands
	int commSize;	//total number of commands	
	char *input[2]; //file for input
	char *output[2]; //file for output
	char *parsedLine[MAX]; //original parsedline
	int parsedSize;
	char *newLine[MAX]; //the newline we want
	int newLineSize;
	int totalPipes;	//total pipes
	//int myArgCount;
	char * args[MAX];
	char ** BigArgs[100];
	int argSize;
};

//Parse Function
struct parse * Parse(char *line);
//print function
void printLine(struct parse *x);
#endif