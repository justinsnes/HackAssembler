#include <string.h>
//#include <stdlib.h>

#include "InstructionLookup.h"

INSTRUCTIONLOOKUP destLookup[] = {
    {"", "000"},    {"M", "001"},   {"D", "010"},   {"MD", "011"}, 
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
    {"D&A", "0000000"}, {"D|A", "0010101"}, {"M", "1110000"},   {"!M", "1110001"},
    {"-M", "1110011"},  {"M+1", "1110111"}, {"M-1", "1110010"}, {"D+M", "1000010"},
    {"D-M", "1010011"}, {"M-D", "1000111"}, {"D&M", "1000000"}, {"D|M", "1010101"}
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
            return record;
        }
    }
}