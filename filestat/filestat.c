/*
Name : Colton Hagan
Date : 1/26/21
Class : CS 347
Program : Takes in a files and output information about
          its file type, time it was last accessed, and permissions
*/

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    
    mode_t file_perm;          /* File permissions */
    struct stat file_details;  /* Detailed file info */
    struct timespec last_time; /* Time of last access */
    char time[1000];           /* Formatted time of last access */

    //Gets the file info    
    stat(argv[1], &file_details);
    file_perm = file_details.st_mode;
    last_time = file_details.st_atim;

    //Gets the file type
    printf("File type : ");
    if(S_ISREG(file_perm)) {
       printf("Regular\n");
    } else if (S_ISDIR(file_perm)){
       printf("Directory\n");
    } else {
       printf("Other\n");
    }

    //Gets the last time accessed
    strftime(time, sizeof(time), "%c", localtime(&last_time.tv_sec));
    printf("Time of last access : %s\n", time);

    //Get the file permissions
    printf("File Permissions:\n");

    //Permissions for user
    printf("User : ");
    if(file_perm & S_IRUSR) {
       printf("Readable, ");
    } else {
       printf("Not readable, ");
    } 
    if(file_perm & S_IWUSR) {
       printf("Writable, ");
    } else {
       printf("Not writable, ");
    }
    if(file_perm & S_IXUSR) {
       printf("Excutable\n");
    } else {
       printf("Not excutable\n");
    }

    //Permissions for group
    printf("Group : ");
    if(file_perm & S_IRGRP) {
       printf("Readable, ");
    } else {
       printf("Not readable, ");
    }
    if(file_perm & S_IWGRP) {
       printf("Writable, ");
    } else {
       printf("Not writable, ");
    }
    if(file_perm & S_IXGRP) {
       printf("Excutable\n");
    } else {
       printf("Not excutable\n");
    }

    //Permissions for other
    printf("Other : ");
    if(file_perm & S_IROTH) {
       printf("Readable, ");
    } else {
       printf("Not readable, ");
    }
    if(file_perm & S_IWOTH) {
       printf("Writable, ");
    } else {
       printf("Not writable, ");
    }
    if(file_perm & S_IXOTH) {
       printf("Excutable\n");
    } else {
       printf("Not excutable\n");
    }
}
