#https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Directories
SRCDIR := ./src
HDRDIR := $(SRCDIR)/include
ODIR   := ./obj
OUTDIR := ./out

# Compiler and flags
CC := gcc
CFLAGS := -I$(HDRDIR) -std=c99 # -Wall -Wextra
LIBS := -lncurses

# Find all .c files in SRCDIR (recursively, if you want, use **/*.c instead)
SRC := $(shell find $(SRCDIR) -type f -name '*.c') #$(wildcard $(SRCDIR)/*.c) 
# could be replaced by plain "main.c"

# Map each .c to its corresponding .o file in ODIR
OBJ := $(patsubst $(SRCDIR)/%.c, $(ODIR)/%.o, $(SRC)) 
# eg './$(src)</lib/test>.c' -> './$(obj)</lib/test>.o' only keep </lib/test>
# could be replaced by plain "main.o"
# Instead get, $SRC and create .o files accordingly : $(patsubst pattern, replacement, text) : /src/<main>.c -> /obj/<main>.o

# Find all .h files (used for dependency checking)
DEPS := $(wildcard $(HDRDIR)/*.h) 
# could be replaced by plain "header.h"

# Final binary output
TARGET := $(OUTDIR)/exec

# Default rule
all: $(TARGET)

## How to build the final executable
$(TARGET): $(OBJ)
	@mkdir -p $(OUTDIR)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
#@echo "Linked -> $@"

## How to compile each object file
# kind of function for each *.o, with corresponding .c file
# $@ -> refers to %.o (<name>:arg)
# $< refers to %.c (name:<arg>)
# $(DEPS) unique purpose is for make to check if they're present and are not directly passed as argument for compilation
$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
# Creates dir of generated object
	@mkdir -p $(dir $@) 
	$(CC) -c -o $@ $< $(CFLAGS)
#@echo "Compiled -> $@"

# Cleaning rules
.PHONY: clean bear

asan: CFLAGS += -fsanitize=address
asan: $(TARGET)

clean:
	rm -rf $(ODIR)/*
	rm -f $(TARGET)
#@echo "Cleaned build artifacts"