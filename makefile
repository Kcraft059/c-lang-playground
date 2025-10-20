#https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

SRCDIR = ./src
OUTDIR = ./out
ODIR = ./obj
IDIR = $(SRCDIR)/include

LIBS = -lm

_DEPS = define.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.c
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

CC = gcc
CFLAGS = -I$(IDIR) #-fsanitize=address


## Needed to include dependencies like func.h
# kind of function for each *.o, with corresponding .c file
# $@ -> refers to %.o (<name>:arg)
# $< refers to %.c (name:<arg>)
# $(DEPS) unique purpose is for make to check if they're present and are not directly passed as argument for compilation

## Make objects with included dependencies 
$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

## Make exec with all objects
$(OUTDIR)/exec: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f $(OUTDIR)/*