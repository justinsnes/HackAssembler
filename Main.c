#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>

#include "Common.h"
#include "InstructionLookup.h"

typedef struct AsmSymbol {
    char SymbolName[40];
    int Address;
} ASMSYMBOL;

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

    // handle symbol creation (labels and variables)
    int lineAddress = 0;
    int variableMemoryAddress = 16;

    ASMSYMBOL *asmSymbols = malloc(sizeof(ASMSYMBOL) * 1000);
    int asmSymbolCount = 0;
    ASMSYMBOL presetSymbols[23] = {
        {"R0", 0}, {"R1", 1},  {"R2", 2},   {"R3", 3},   {"R4", 4},   {"R5", 5},         {"R6", 6},     {"R7", 7},
        {"R8", 8}, {"R9", 9},  {"R10", 10}, {"R11", 11}, {"R12", 12}, {"R13", 13},       {"R14", 14},   {"R15", 15}, 
        {"SP", 0}, {"LCL", 1}, {"ARG", 2},  {"THIS", 3}, {"THAT", 4}, {"SCREEN", 16384}, {"KBD", 24576}
    };
    for (int x = 0; x < 23; x++) {
        strcpy(asmSymbols[x].SymbolName, presetSymbols[x].SymbolName);
        asmSymbols[x].Address = presetSymbols[x].Address;
        //asmSymbols++;
        asmSymbolCount++;
    }

    // iterate to setup all labels
    char *lineNoWhitespace;
    while((read = getline(&line, &len, fpasm)) != -1)
    {
        // remove white spaces
        lineNoWhitespace = str_replace(line, " ", "");
        // ignore comments and empty lines
        if (*lineNoWhitespace == '\r' || *lineNoWhitespace == '/')
        {
            free(lineNoWhitespace);lineNoWhitespace = NULL;
            continue;
        }

        if (*lineNoWhitespace == '(')
        {
            char *label = getCharsBetween(lineNoWhitespace, "(", ")");
            strcpy(asmSymbols[asmSymbolCount].SymbolName, label);
            asmSymbols[asmSymbolCount].Address = lineAddress;
            asmSymbolCount++;
        } else {
            lineAddress++;
        }
    }

    rewind(fpasm);
    // convert the rest of assembly to machine code
    while((read = getline(&line, &len, fpasm)) != -1)
    {
        // remove white space
        lineNoWhitespace = str_replace(line, " ", "");
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

            sscanf(cleanAddress, "%d", &aAddr);

            if (aAddr == 0 && *cleanAddress != '0') {
                // look for a symbol match since it's not a number
                bool symbolFound = false;
                for (int x = 0; x < asmSymbolCount; x++) {
                    ASMSYMBOL record = asmSymbols[x];
                    if (strcmp(cleanAddress, record.SymbolName) == 0)
                    {
                        aAddr = record.Address;
                        symbolFound = true;
                        break;
                    }
                }
                // create symbol entry if it doesn't exist
                if (!symbolFound) {
                    aAddr = variableMemoryAddress;
                    strcpy(asmSymbols[asmSymbolCount].SymbolName, cleanAddress);
                    asmSymbols[asmSymbolCount].Address = aAddr;
                    variableMemoryAddress++;
                    asmSymbolCount++;
                }
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
        
    free(asmSymbols); asmSymbols = NULL;

    exit(EXIT_SUCCESS);
}

