#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Common.h"
#include "InstructionLookup.h"

typedef struct AsmLabel {
    char LabelName[20];
    int LineNumber;
} ASMLABEL;

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

    // handle label creation
    int lineNumber = 1;
    ASMLABEL *asmLabels = malloc(sizeof(ASMLABEL) * 20);
    int asmLabelCount = 0;
    while((read = getline(&line, &len, fpasm)) != -1)
    {
        // remove white space
        char *lineNoWhitespace = str_replace(line, " ", "");
        // ignore comments and empty lines
        if (*lineNoWhitespace == '\r' || *lineNoWhitespace == '/')
        {
            free(lineNoWhitespace);lineNoWhitespace = NULL;
            continue;
        }

        if (*lineNoWhitespace == '(')
        {
            char *label = getCharsBetween(lineNoWhitespace, "(", ")");
            strcpy(asmLabels->LabelName, label);
            asmLabels->LineNumber = lineNumber;
            asmLabels++;
            asmLabelCount++;
        }
        else {
            lineNumber++;
        }
    }

    // set the true amount of memory used for the labels
    // reset to the correct, original pointer address.
    asmLabels = asmLabels - asmLabelCount; 
    ASMLABEL *trueAsmLabels = realloc(asmLabels, asmLabelCount * sizeof(ASMLABEL));
    if (trueAsmLabels == NULL) {
        printf("\nMemory Allocation reallocation failed. ABORT!");
        free (asmLabels);asmLabels = NULL;
        exit(0);
    }
    asmLabels = trueAsmLabels;

    rewind(fpasm);
    // convert the rest of assembly to machine code
    while((read = getline(&line, &len, fpasm)) != -1)
    {
        // remove white space
        char *lineNoWhitespace = str_replace(line, " ", "");
        // ignore comments, empty lines and labels we already processed.
        if (*lineNoWhitespace == '\r' || *lineNoWhitespace == '/' || *lineNoWhitespace == '(')
        {
            free(lineNoWhitespace);lineNoWhitespace = NULL;
            continue;
        }
        
        char machinecode[17]; // leave room for null-terminate char
        // A or C instruction
        if (*lineNoWhitespace == '@')
        {
            unsigned int aAddr = 0;
            char *cleanAddress = str_replace(lineNoWhitespace+1, "\r\n", "");

            // look for a label match
            for (int x = 0; x < asmLabelCount; x++) {
                ASMLABEL record = asmLabels[x];
                if (strcmp(cleanAddress, record.LabelName) == 0)
                {
                    aAddr = record.LineNumber;
                    break;
                }
            }
            // look for a register match
            if (aAddr == 0) {
                char *registerTranslated = str_replace(cleanAddress, "R", "");
                sscanf(registerTranslated, "%d", &aAddr);
                free(registerTranslated);registerTranslated = NULL;
            }
            
            char *aInstruct = toBinaryString(aAddr, 16);
            strcpy(machinecode, aInstruct);

            free(aInstruct);aInstruct = NULL;
            free(cleanAddress);cleanAddress = NULL;
        }
        else
        {
            strcpy(machinecode, "111accccccdddjjj\0");
            char *cleanLine = str_replace(lineNoWhitespace, "\r\n", "");
            
            // destination
            char *destination = getCharsBetween(cleanLine, "", "=");
            INSTRUCTIONLOOKUP destResult = findBinaryInstruction(destination, 'd');
            char *destTemp = str_replace(machinecode, "ddd", destResult.binary);
            strcpy(machinecode, destTemp);
            free(destTemp);destTemp = NULL;

            // jump
            char *jump = getCharsBetween(cleanLine, ";", "");
            INSTRUCTIONLOOKUP jumpResult = findBinaryInstruction(jump, 'j');
            char *jumpTemp = str_replace(machinecode, "jjj", jumpResult.binary);
            strcpy(machinecode, jumpTemp);
            free(jumpTemp);jumpTemp = NULL;

            // computation
            char *comp;
            // if there's no destination, then there's no "=" to search from
            if (*destination == 0) {
                comp = getCharsBetween(cleanLine, "", ";");
            } else {
                comp = getCharsBetween(cleanLine, "=", ";");
            }
            INSTRUCTIONLOOKUP compResult = findBinaryInstruction(comp, 'c');
            char *compTemp = str_replace(machinecode, "acccccc", compResult.binary);
            strcpy(machinecode, compTemp);
            free(compTemp);compTemp = NULL;

            free(destination);destination = NULL;
            free(jump);jump = NULL;
            free(comp);comp = NULL;
            free(cleanLine);cleanLine = NULL;
        }

        free(lineNoWhitespace);lineNoWhitespace = NULL;
        fprintf(fphack, "%s\n", machinecode);
        
    }

    fclose(fpasm);
    if (line) {
        free(line);line = NULL;
    }
        
    free(asmLabels); asmLabels = NULL;

    exit(EXIT_SUCCESS);
}

