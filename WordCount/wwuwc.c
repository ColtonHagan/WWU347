/*
Name : Colton Hagan
Date : 1/19/21
Class : CS 347
Program : Reads in a string and outputs the number of chars, lines, and
          words contained in the string similar to wc unix function
*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define FALSE (0)
#define TRUE  (1)

int main() {
	int tot_chars = 0 ;	/* total characters */
	int tot_lines = 0 ;	/* total lines */
	int tot_words = 0 ;	/* total words */
	int curr_char; /* char you are reading */
	int has_word = 0; /* if we have had a word */
	
	//Reads the chars from an input untill an EOF
	while((curr_char = getchar()) != EOF) {
	    //Since we are reading chars each we can simply increment here
	    tot_chars++;
	    /*if there is a whitespace we know that
	      the prevous chars made up a word */
	    if(isspace(curr_char)) {
	    	if(has_word == 1) {
		    tot_words++;
		    has_word = 0;
		}
	    } else {
		has_word = 1;
	    }
	    /*There is a line if there is a new line symbol
	      or if the input ends (since it will always be on a line*/
	    if(curr_char == '\n' || curr_char == '\0') {
	        tot_lines++;
	    }
	}
    	//Prints output
    	printf(" %d %d %d\n", tot_lines, tot_words, tot_chars);
    	return 0;
}
