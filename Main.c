#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Common.h"

typedef struct InstructionLookup {
    char *assembly;
    char *binary;
} INSTRUCTIONLOOKUP;

INSTRUCTIONLOOKUP destLookup[] = {
    {"", "000"},    {"M", "001"},   {"D", "010"},   {"DM", "011"}, 
    {"A", "100"},   {"AM", "101"},  {"AD", "110"},  {"ADM", "111"}
};

INSTRUCTIONLOOKUP jumpLookup[] = {
    {"", "000"},    {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"}, 
    {"JLT", "100"}, {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"}
};

INSTRUCTIONLOOKUP compLookup[] = {
    {"0", "0101010"},   {"1", "0111111"},   {"-1", "0111010"},  {"D", "0001100"}, 
    {"A", "0110000"},   {"!D", "0001101"},  {"!A", "0110001"},  {"-D", "0001111"}, 
    {"-A", "0110001"},  {"D+1", "0011111"}, {"A+1", "0110111"}, {"D-1", "0001110"}, 
    {"A-1", "0110010"}, {"D+A", "0000010"}, {"D-A", "0010011"}, {"A-D", "0000111"}, 
    {"D&A", "0000000"}, {"D|A", "0010101"}
};

INSTRUCTIONLOOKUP findBinaryInstruction(char *value, char actionCode) {
    INSTRUCTIONLOOKUP *lookup;
    int arrCount;
    switch (actionCode)
    {
        case 'd': 
            lookup = destLookup;
            arrCount = sizeof(destLookup) / sizeof(destLookup[0]);
            break;
        case 'j': 
            lookup = jumpLookup; 
            arrCount = sizeof(jumpLookup) / sizeof(jumpLookup[0]);
            break;
        case 'c': 
            lookup = compLookup; 
            arrCount = sizeof(compLookup) / sizeof(compLookup[0]);
            break;
        default: lookup = NULL;
    }
    for (int x = 0; x < arrCount; x++) {
        INSTRUCTIONLOOKUP record = lookup[x];
        if (strcmp(value, record.assembly) == 0)
        {
            int debug2 = 2 + 2;
            return record;
        }
        int debug1 = 1 + 1;
    }
}

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
            //char *output;
            //unsigned long result = strtoul(cleanNumber, &output, 10) + 1;
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

