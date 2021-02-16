// File contains the commands for the shell, and a way to check if the shell conatins a certain command
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "builtin.h"
#include <string.h>
#include <sys/stat.h>

static void exitProgram(char** args, int argcp);
static void cd(char** args, int argcp);
static void pwd(char** args, int argcp);
static void makedir(char** args, int argcp);

/* Checks if given command is contained in shell. If it is 
   runs command and returns 1, if it is not returns 0*/
int builtIn(char** args, int argcp) {
	if(strcmp(args[0], "exit") == 0) {
		exitProgram(args, argcp);
		return 1;
	} else if (strcmp(args[0], "pwd") == 0) {
		pwd(args, argcp);
		return 1;
	} else if (strcmp(args[0], "cd") == 0) {
		cd(args, argcp);
		return 1;
	} else if (strcmp(args[0], "mkdir") == 0) {
		makedir(args, argcp);
		return 1;
	}
	return 0;
}

/*exits from the shell with given value,
  or if value is not given exits with 0*/
static void exitProgram(char** args, int argcp) {
	if(argcp > 1) {
		exit(atoi(args[1]));
	} else {
		exit(0);
	}
}

//Prints the current working directory
static void pwd(char** args, int argpc) {
	char cwd[100];
	printf("%s\n", getcwd(cwd, sizeof(cwd)));
}

//Moves the user's directory to a given new directory
static void cd(char** args, int argcp) {
	if(argcp > 1) {
		chdir(args[1]);
	} else {
		/*Emulates the cd functionality of returning
		  to home directoryy if no new directory is given*/
		chdir(getenv("HOME"));
	}
}
/*Makes a new given directory, if no directory is given
  or it is already created prints error*/
static void makedir(char** args, int argcp) {
	struct stat buffer;
	//uses stat to check if file exists
	if (stat(args[1], &buffer) != 0 && argcp > 1) {
		mkdir(args[1], 0777);
	} else if (argcp == 1) {
		printf("ERROR: Needs name of file to create\n");
	} else {
		printf("ERROR: File is already created\n");
	}
}
