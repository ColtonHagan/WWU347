/*
Name : Colton Hagan
Date : 1/22/2021
Class : CS 347
Program : Reads or decodes a file by shifting charcters by 100 askii letters
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv) {

    char outputFile[128];
    char* command = argv[1];
    char *file = argv[2];
    
    // If there is an incorrect input informs user proper usage
    if(strcmp(argv[1], "-e") != 0 && strcmp(argv[1], "-d") != 0) {
        printf("Usage:\n./filesec -e|-d [filename]\n");
    }
    
    // Encodes new file
    if (strcmp(argv[1], "-e") == 0) {
        int pFile = open(file, O_RDONLY);
        strcpy(outputFile, file);
        strtok(outputFile, ".");
        strcat(outputFile, "_enc.txt");
        int encoded_file = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        char *current_char[0];
        // Reads and writes output
        while (read(pFile, current_char, 1) == 1) {
           char shifted = *current_char + 100;
           write(encoded_file, &shifted, 1);
        }

        close(pFile);
        close(encoded_file);
        return 0;
    }
    // Decodes into new file
    if (strcmp(argv[1], "-d") == 0) {
        int pFile = open(file, O_RDONLY);
        strcpy(outputFile, file);
        strtok(outputFile, ".");
        strcat(outputFile, "_dec.txt");
        int decoded_file = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        printf("%d\n", decoded_file);
        char *current_char[0];
        // Reads and writes output
        while (read(pFile, current_char, 1) == 1) {
           char shifted = *current_char - 100;
           write(decoded_file, &shifted, 1);
        }

        close(pFile);
        close(decoded_file);
        return 0;
    }

}
