#ifndef INSTRUCTIONLOOKUP_H
#define INSTRUCTIONLOOKUP_H

typedef struct InstructionLookup {
    char *assembly;
    char *binary;
} INSTRUCTIONLOOKUP;

INSTRUCTIONLOOKUP findBinaryInstruction(char *value, char actionCode);

#endif // INSTRUCTIONLOOKUP_H