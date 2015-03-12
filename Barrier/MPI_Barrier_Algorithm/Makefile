OMPFLAGS = -fopenmp
OMPLIBS = -lgomp

MPICC = mpicc
CCFLAGS = -Wall -std=c99 -I/usr/lib64/openmpi/1.4-gcc/include

all: test_tournament test_dissemination test_native
.PHONY: all

test_tournament: test_tournament.o timer.o tournament.o
	$(MPICC) $(CCFLAGS) -o $@ $^

test_dissemination: test_dissemination.o timer.o dissemination.o
	$(MPICC) $(CCFLAGS) -o $@ $^

test_native: test_native.o timer.o
	$(MPICC) $(CCFLAGS) -o $@ $^

timer.o:
	$(MPICC) $(CCFLAGS) -c timer.c

test_tournament.o:
	$(MPICC) $(CCFLAGS) -c test_tournament.c

test_dissemination.o:
	$(MPICC) $(CCFLAGS) -c test_dissemination.c

test_native.o:
	$(MPICC) $(CCFLAGS) -c test_native.c

dissemination.o:
	$(MPICC) $(CCFLAGS) -c dissemination.c

tournament.o:
	$(MPICC) $(CCFLAGS) -c tournament.c
clean:
	rm -f *.o
