OMPFLAGS = -fopenmp
OMPLIBS = -lgomp -lm

CC = gcc
CPPFLAGS = -g -Wall $(OMPFLAGS)
LDFLAGS = -g -Wall
LDLIBS = $(OMPLIBS)

all: test

test: test.o MCS.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o test
