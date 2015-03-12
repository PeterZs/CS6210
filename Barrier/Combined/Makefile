OMPFLAGS = -fopenmp
OMPLIBS = -lgomp -lm

MPICC = mpicc
CCFLAGS = -Wall -std=c99 -I/usr/lib64/openmpi/1.4-gcc/include $(OMPFLAGS) 
LDLIBS = $(OMPLIBS)

MPI_OMP_Test: Combined_Test.o CombinedBarrier.o 
	$(MPICC) $(CCFLAGS) $(LDLIBS) -o $@ $^

Combined_Test.o:
	$(MPICC) $(CCFLAGS) $(LDLIBS) -c Combined_Test.c

CombinedBarrier.o:
	$(MPICC) $(CCFLAGS) $(LDLIBS) -c CombinedBarrier.c

clean:
	rm -f *.o

