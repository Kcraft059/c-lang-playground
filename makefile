#https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC = gcc
CFLAGS = -I.
DEPS = func.h
OBJ = test.o func.o

## Needed to include dependencies like func.h
# kind of function for each *.o, with corresponding .c file
# $@ -> refers to %.o (<name>:arg)
# $< refers to %.c (name:<arg>)
# $(DEPS) unique purpose is for make to check if they're present and are not directly passed as argument for compilation
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

prog: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
