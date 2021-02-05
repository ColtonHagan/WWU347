
/*
Name : Colton Hagan
Date : 2/1/21
Class : CS 347
Program : Copies one file to another file
*/

#include <stdio.h>                /* Standard library for file operations */
#include <string.h>               /* String functions (strcat) */
#define BUF_SIZE 32               /* Size of buffer */
 
int main(int argc, char *argv[]) {
    FILE * file_to_read;        // File to be copied
    FILE * file_to_write;       // Copy of the file
       
    char buf[BUF_SIZE];        // Buffer to hold characters read
    size_t num_rec;           // Number of records to be read/written

    // Check if correct number of arguments are supplied
    if(argc != 2 && argc != 3) {
       printf("Usage : \n ./copyfile <src_file_name> [<target_file_name>]");
    }

    // Prepares file names
    char read_file_name[1000];
    strcpy(read_file_name, argv[1]);
    char write_file_name[1000];
    if(argc == 2) {
       strcpy(write_file_name, read_file_name);
       strcat(write_file_name, ".output");
    } else {
       strcpy(write_file_name, argv[2]);
    }

    // Open files and writes error message if they do not work
    file_to_read = fopen(read_file_name,"r");
    if(file_to_read == NULL) {
        printf("ERROR : Failed to open file you are copying from.\n");
	return 0;
    }
    file_to_write = fopen(write_file_name,"w");
    if(file_to_write == NULL) {
        printf("ERROR : Failed to open file you are copying to.\n");
	return 0;
    }

    //Reads and writes the file
    while((num_rec = fread(buf, 1, BUF_SIZE, file_to_read)) > 0) {
        fwrite(buf, 1, num_rec, file_to_write);
    }
    
    // Close the files
    fclose(file_to_read);
    fclose(file_to_write);
    return 0;
}

