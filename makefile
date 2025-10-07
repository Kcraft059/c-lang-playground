#https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

OUTDIR = ./out
ODIR = ./obj
IDIR = ./include

LIBS = -lm

_DEPS =
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = test.o func.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

CC = gcc
CFLAGS = -I$(IDIR)


## Needed to include dependencies like func.h
# kind of function for each *.o, with corresponding .c file
# $@ -> refers to %.o (<name>:arg)
# $< refers to %.c (name:<arg>)
# $(DEPS) unique purpose is for make to check if they're present and are not directly passed as argument for compilation

## Make objects with included dependencies 
$(ODIR)/%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

## Make exec with all objects
$(OUTDIR)/exec: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f $(OUTDIR)/*
