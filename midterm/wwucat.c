/*
Name: Colton Hagan
Date : 2/16/21
Class : cs 347
Program : Midterm project, replicates the unix cat command
*/
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define BUF_SIZE 32               /* Size of buffer */
#define FILE_NAME_LEN 200         /* Maximum length of file name */

int main(int argc, char *argv[]) {
	if(argc > 1) {
		for(int i = 1; i < argc; i++) {
			char file[FILE_NAME_LEN]; //File
			char buf[BUF_SIZE]; // buffer
			int reading;
			int writing;
			//reads file
			strcpy(file, argv[i]);
			int pFile = open(file, O_RDONLY);
			if(pFile < 0) {
				printf("Error opening the file for reading\n");
				continue;
			}
			// Reads and writes output
			while ((reading = read(pFile, &buf, BUF_SIZE)) != 0) {
				if(reading > 0) {
					write(1, &buf, reading);
					if(writing < 0) {
						printf("Error while writing to standard output\n");
						break;
					}
				} else {
					printf("Error while reading the file\n");
					break;
				}
			}
			//closes file
			close(pFile);
		}
	} else {
		printf("Expecting 1 filename\n");
	}
	return 0;
}
