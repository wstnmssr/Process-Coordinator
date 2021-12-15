#list of files
C_SRCS = Coordinator.c Checker.c
C_OBJS = Coordinator.o Checker.o
#headers
OBJS = ${C_OBJS}
EXE1 = Coordinator
EXE2 = Checker
#compile, load commands w/ flags
GCC = gcc
GCC_FLAGS = -std=c11 -g -Wall -c -I.
LD_FlAGS = -std=c11 -g -Wall -I.
#compile .c to .o
.c.o:
	$(GCC) $(GCC_FLAGS) $<

all: Coordinator Checker
Coordinator: Coordinator.o
	$(GCC) $(LD_FLAGS) Coordinator.o -o $(EXE1)
Checker: Checker.o
	$(GCC) $(LD_FLAGS) Checker.o -o $(EXE2)

#recompile if new headers
$(C_OBJS): ${C_HEADERS}

#directory maintenance
clean:
	rm -f *.o *~ $(EXE1) $(EXE2)

pack:
	zip -v Westin-Musser-HW3.zip Coordinator.c Checker.c Checker.h Makefile README.txt
