#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include "Common.h"

char *str_replace(char *orig, char *rep, char *with);

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

    char *fphack_filename = str_replace(filename, ".asm", ".hack");
    FILE *fphack = fopen(fphack_filename, "w");
    free (fphack_filename);
    fphack_filename = NULL;
    while((read = getline(&line, &len, fpasm)) != -1)
    {
        // ignore comments and white space
        if (*line == '\r' || *line == '/')
            continue;
        fprintf(fphack, "%s", line);
    }

    fclose(fpasm);
    if (line)
        free(line);

    exit(EXIT_SUCCESS);
}