CC = gcc
CFLAGS = -c
#OUTPUTFLAGS = 
OUTPUTFLAGS = -g3 -O0 # for debugging
RM = rm -f

all: HackAssembler

HackAssembler: Main.o InstructionLookup.o Common.o
	$(CC) Main.o InstructionLookup.o Common.o $(OUTPUTFLAGS) -o HackAssembler
	chmod +x HackAssembler

Main.o: Main.c
	$(CC) $(CFLAGS) Main.c $(OUTPUTFLAGS)

InstructionLookup.o: InstructionLookup.c InstructionLookup.h
	$(CC) $(CFLAGS) InstructionLookup.c $(OUTPUTFLAGS)

Common.o: Common.c Common.h
	$(CC) $(CFLAGS) Common.c $(OUTPUTFLAGS)

target: dependencies
	action

clean: 
	rm *.o