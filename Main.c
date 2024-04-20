#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Common.h"
#include "InstructionLookup.h"

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
        
        char machinecode[17]; // leave room for null-terminate char
        // A or C instruction
        if (*line == '@')
        {
            char *cleanNumber = str_replace(line+1, "\r\n", "");
            
            unsigned int aAddr;
            sscanf(cleanNumber, "%d", &aAddr);

            char *aInstruct = toBinaryString(aAddr, 16);
            strcpy(machinecode, aInstruct);

            free(aInstruct);aInstruct = NULL;
            free(cleanNumber);cleanNumber = NULL;
        }
        else
        {
            strcpy(machinecode, "111accccccdddjjj\0");
            char *cleanLine = str_replace(line, "\r\n", "");
            
            // destination
            char *destination = getCharsBetween(cleanLine, "", "=");
            INSTRUCTIONLOOKUP destResult = findBinaryInstruction(destination, 'd');
            char *destTemp = str_replace(machinecode, "ddd", destResult.binary);
            strcpy(machinecode, destTemp);
            free(destination);destination = NULL;
            free(destTemp);destTemp = NULL;

            // jump
            char *jump = getCharsBetween(cleanLine, ";", "");
            INSTRUCTIONLOOKUP jumpResult = findBinaryInstruction(jump, 'j');
            char *jumpTemp = str_replace(machinecode, "jjj", jumpResult.binary);
            strcpy(machinecode, jumpTemp);
            free(jump);jump = NULL;
            free(jumpTemp);jumpTemp = NULL;

            // computation
            char *comp = getCharsBetween(cleanLine, "=", ";");
            INSTRUCTIONLOOKUP compResult = findBinaryInstruction(comp, 'c');
            char *compTemp = str_replace(machinecode, "acccccc", compResult.binary);
            strcpy(machinecode, compTemp);
            free(comp);comp = NULL;
            free(compTemp);compTemp = NULL;

            free(cleanLine);cleanLine = NULL;
        }

        fprintf(fphack, "%s\n", machinecode);
        
    }

    fclose(fpasm);
    if (line)
        free(line);

    exit(EXIT_SUCCESS);
}

