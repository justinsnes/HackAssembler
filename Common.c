#include <string.h>
#include <stdlib.h>

#include "Common.h"

// copied from Stack Overflow - jmucchiello (https://stackoverflow.com/questions/779875/what-function-is-to-replace-a-substring-from-a-string-in-c)
// You must free the result if result is non-NULL.
// char *done = replace("abcdefghijkl", "bc", "yz"); do_stuff(); free(done);
char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

char *getCharsBetween(char *charsValue, char *before, char *after) {
    char *start = strstr(charsValue, before);
    if (start == 0x0) {
        start = strstr(charsValue, "");
    } else { 
        start += strlen(before);
    }
    
    char *end = strstr(start, after);
    if (end == 0x0) {
        end = &start[strlen(start)]; //strstr(start, "\0");
        //end - 1; // we add the null terminator back in later anyway.
    }
    char *retval = (char*)malloc(end - start + 1);
    memcpy(retval, start, end - start);
    retval[end - start] = '\0';
    return retval;
}

// Free the result after use.
char *toBinaryString(int n, int num_bits) {
    char *string = malloc(num_bits + 1);
    if (!string) {
        return NULL;
    }

    for (int i = num_bits - 1; i >= 0; i--) {
        // grab LSB with (n & 1) and add '0' to convert the bit value to string. 
        // if bit is 1, it adds that to the ascii value which makes (48 + 1) = 1's value in ASCII
        string[i] = (n & 1) + '0';
        // right-shift bits to get next bit in the sequence 
        n >>= 1;
    }
    string[num_bits] = '\0';
    return string;
}