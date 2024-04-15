#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

typedef struct {

} thestruct;

typedef union {

} theunion;

int main(int argc, char **argv)
{
    char *filename = argv[1];
    printf("File name: %s\n", filename);

    char *line = NULL;
    // use size_t instead of int for arbitrary object/string sizes
    size_t len = 0;
    ssize_t read;

    FILE *fpasm = fopen(filename, "r");
    if (fpasm == NULL)
    {
        printf("File not found.\n");
        exit(EXIT_FAILURE);
    }

    char *fphack_filename = filename;
    FILE *fphack = fopen("", "w");
    while((read = getline(&line, &len, fpasm)) != -1)
    {
        if (line == '\r')
            continue;
        printf("%s", line);
        //printf("%d\n", *line);
    }

    fclose(fpasm);
    if (line)
        free(line);

    exit(EXIT_SUCCESS);
}

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