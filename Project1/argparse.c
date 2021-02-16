// File splits a string into array elements based on whitespace
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "argparse.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define FALSE (0)
#define TRUE  (1)

static int argCount(char* line);

//Finds and returns number of words/arguments in a string
static int argCount(char* line) {
	int words = 0;
	int has_word = 0; //if last char was not a whitespace
	int chs = 0;
	//reads string until it ends
	while(line[chs] != '\0') {
		/*if there is a space and the last charcter
		  was a not a space assumes it is a word*/
		if(isspace(line[chs])) {
			if(has_word) {
				words++;
				has_word = 0;
			}
		} else {
			has_word = 1;
		}
		chs++;
	}
	return words;
}

//Splits a string into an array based on whitespace
char** argparse(char* line, int* argcp) {
	char **args;
	*argcp = argCount(line);
	args = malloc(*argcp);
	char arg[100] = "";
	int current_arg = 0;
	int chs = 0;
	int has_word = 0; //if last char was not a whitespace
	while(line[chs] != '\0') {
		//if space
		if(isspace(line[chs])) {
			/*if there is a space and the last charcter
			  was a not a space assumes it is a word*/
			if(has_word) {
				//adds word to array
				args[current_arg] = malloc(strlen(arg));
				strcpy(args[current_arg], arg);
				current_arg++;
				memset(arg,0,strlen(arg));
				has_word = 0;
			}
		//if char
		} else {
			arg[strlen(arg)] = line[chs];
			has_word = 1;
		}
		chs++;
	}
	return args;
}
