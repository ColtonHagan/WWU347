/*
 * Implementation of functions that find values in strings.
 *****
 * YOU MAY NOT USE ANY FUNCTIONS FROM <string.h>
 *****
 */

#include <stdlib.h>
#include <stdbool.h>

#include "find.h"

#define NOT_FOUND (-1)	// integer indicator for not found.

/*
 * Return the index of the first occurrence of <ch> in <string>,
 * or (-1) if the <ch> is not in <string>.
 */
int find_ch_index(char string[], char ch) {
    int i = 0;
    //looks through string to see if char occurs
    while(string[i] != '\0') {
        if(string[i] == ch) {
            return i;
        }
        i++;
    }
	return -1;
}

/*
 * Return a pointer to the first occurrence of <ch> in <string>,
 * or NULL if the <ch> is not in <string>.
 *****
 * YOU MAY *NOT* USE INTEGERS OR ARRAY INDEXING.
 *****
 */
char *find_ch_ptr(char *string, char ch) {
    //looks through pointer string to see if char occurs
    while(*(string) != '\0') {
        if(*(string) == ch) {
            return string;
        }
        string++;
    }
    return NULL;
}

/*
 * Return the index of the first occurrence of any character in <stop>
 * in the given <string>, or (-1) if the <string> contains no character
 * in <stop>.
 */
int find_any_index(char string[], char stop[]) {
    int i = 0;
    //looks through string to see if any part of stop matchs
	while(string[i] != '\0') {
        int j = 0;
        while(stop[j] != '\0') {
            if(string[i] == stop[j] ) {
                return i;
            }
            j++;
        }
        i++;
    }
    return -1;
}

/*
 * Return a pointer to the first occurrence of any character in <stop>
 * in the given <string> or NULL if the <string> contains no characters
 * in <stop>.
 *****
 * YOU MAY *NOT* USE INTEGERS OR ARRAY INDEXING.
 *****
 */
char *find_any_ptr(char *string, char* stop) {
    //looks through pointer string to see if any part of stop matchs
	while(*(string) != '\0') {
	    //needed so we can start over at start of pointer if no same char is found
        char* temp_stop = stop;
        while(*(temp_stop) != '\0') {
            if(*(string) == *(temp_stop)) {
                return string;
            }
            temp_stop++;
        }
        string++;
    }
    return NULL;
}

/*
 * Return a pointer to the first character of the first occurrence of
 * <substr> in the given <string> or NULL if <substr> is not a substring
 * of <string>.
 * Note: An empty <substr> ("") matches *any* <string> at the <string>'s
 * start.
 *****
 * YOU MAY *NOT* USE INTEGERS OR ARRAY INDEXING.
 *****
 */
char *find_substr(char *string, char* substr) {
    //checks to see if the substring is empty
    if(substr == '\0') {
        return string;
    }
    //looks through pointer string to see if pointer substring is in it
	while(*(string) != '\0') {
	    /*if the first chars of the pointers match checks to see if 
	    the rest of the substring is also in string*/
        if(*(string) == *(substr)) {
            char* temp_substring = substr;
            char* temp_string = string;
            //Acts as boolean set to TRUE
            int sub_string = 1;
            while(*(temp_substring) != '\0') {
                //If a charcter doesn't match changes "boolean" to false
                if(*(temp_string) != *(temp_substring)) {
                    sub_string = 0;
                }
                temp_substring++;
                temp_string++;
            }
            if(sub_string == 1) {
                return string;
            }
        } 
        string++;
    }
    return NULL;
}
