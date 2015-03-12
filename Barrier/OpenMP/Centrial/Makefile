OMPFLAGS = -fopenmp
OMPLIBS = -lgomp -lm

CC = gcc
CPPFLAGS = -g -Wall $(OMPFLAGS)
LDFLAGS = -g -Wall
LDLIBS = $(OMPLIBS)

all: test

test: test.o CentralBarrier.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o test
