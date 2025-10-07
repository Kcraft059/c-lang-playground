#https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC=gcc
CFLAGS=-I.
DEPS = func.h

## Needed to include dependencies like func.h
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: test.o func.o 
	$(CC) -o test test.o func.o 