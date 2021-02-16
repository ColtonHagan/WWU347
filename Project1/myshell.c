/* CS 347 -- Mini Shell!
 * Original author Phil Nelson 2000
 * Name : Colton Hagan
 * Date : 2/15/21
 * Program : Creates a microshell myshell with a small number of
 *           of built in commands
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "argparse.h"
#include "builtin.h"

void processline (char *line);
ssize_t getinput(char** line, size_t* size);

int main () {
	size_t size = 64;
	char* line = malloc(size);
	//runs untill we get exit from user
	while(1) {
		getinput(&line, &size);
		processline(line);
	}
	return EXIT_SUCCESS;
}

// Prompts and reads input from user in the command line
ssize_t getinput(char** line, size_t* size) {
	assert(line != NULL && size != NULL);
	printf("%% ");
	ssize_t len = getline(line, size, stdin);
	return len;
}

// Exucates command found in given line
void processline (char *line) {
	//checks if a command was entered/line was empty
	if(line[0] == '\0' || line[0] == '\n' || line == NULL) {
		printf("ERROR: Please enter a command\n");
		return;
	}
	pid_t cpid;
	int status;
	int argCount;
	char** arguments = argparse(line, &argCount);
	if(!builtIn(arguments, argCount)) {
		//forks and exucutes unix command
		cpid = fork();
		if(cpid < 0) {
			printf("ERROR: Fork failed\n");
		} else if (cpid == 0) {
			execvp(arguments[0], arguments);
			printf("ERROR: Command is not part of unix or myshell\n");
			exit(0);
		} else {
			wait(&status);
		}
	}
}
