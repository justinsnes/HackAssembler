# HackAssembler
Assembler built for generating binary output for the nand2tetris Hack computer

Build command: make

Run command: ./HackAssembler [FILENAME]
(example: ./HackAssembler ./Asm/Add.asm)

To support VS Code debugging, by default, the OUTPUTFLAGS are set to -g3 -O0 to turn off optimization and turn on debug symbols.